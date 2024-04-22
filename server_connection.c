#define _XOPEN_SOURCE 500
#include <stdio.h>			/*printf, perror*/
#include <string.h>			/*memset*/
#include <unistd.h>         /*sleep*/
#include <sys/select.h>		/*listen for socket activity*/
#include <unistd.h>         /*read*/
#include <strings.h>        /*bzero*/
#include <signal.h>		    /*sig_atomic*/
#include "vpn.h"

#define MAX_MSG_LEN 1000
#define SERVER_MESSAGE "Hello from server\n"

static sig_atomic_t g_run = 1;
/*used when pressed ctrl^c to send interrupt signal
if keyboard interrupted with signal, select() sets flag EINTR with the number 
of the return value in select() and returns back to Init().*/
void KeyboardSignalHandler()
{
    g_run = 0;
    ResetRouting(0);
}

/*Server watches for connections, if connection initiated and client unresponsive,
release resources and terminate*/
int ServerConnection(vpn_socket_handle_t *vpn_sock_handle)
{
    struct timeval timeout;
    char client_message[MAX_MSG_LEN] = {0};
    struct sockaddr_in client_addr = {0};
    socklen_t client_addrlen = sizeof(client_addr);
    
    signal(SIGINT, KeyboardSignalHandler);

    /*receive from client*/
    if (recvfrom(vpn_sock_handle->sockfd, client_message, sizeof(client_message), 0,
    (struct sockaddr*)&client_addr, &client_addrlen) < 0)
    {
        perror("handle_conn (recv)");
        return -1;
    }

    Log();
    fflush(stdout);
    printf("Received from peer: %s:%i\n",
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    printf("Message from client: %s", client_message);

    /*respond to client*/
    if (sendto(vpn_sock_handle->sockfd, SERVER_MESSAGE, sizeof(SERVER_MESSAGE), 0,
    (struct sockaddr*)&client_addr, client_addrlen) < 0)
    {
        perror("handle_conn (send)");
        return -1;
    }

    printf("process running...\n");

    SetRouting(0);

    while (g_run) 
    {
        fd_set readFDSet;
        int maxfd = (vpn_sock_handle->tunfd > vpn_sock_handle->sockfd) ? 
        vpn_sock_handle->tunfd : vpn_sock_handle->sockfd;

        timeout.tv_sec = 12;
        timeout.tv_usec = 0;

        FD_ZERO(&readFDSet);
        FD_SET(vpn_sock_handle->sockfd, &readFDSet);
        FD_SET(vpn_sock_handle->tunfd, &readFDSet);
        
        /*when select returns readFDSet will be modified to reflect on which fd was activity*/
        if (select(maxfd + 1, &readFDSet, NULL, NULL, &timeout) == -1)
        {
            perror("select");
            return -1;
        }
        /*if fd is set, that means there is event on this fd*/
        if (FD_ISSET(vpn_sock_handle->tunfd, &readFDSet))
        {
            TunSelected(vpn_sock_handle->tunfd, vpn_sock_handle->sockfd, &client_addr);
        }
        else if (FD_ISSET(vpn_sock_handle->sockfd, &readFDSet))
        {
            SockSelected(vpn_sock_handle->tunfd, vpn_sock_handle->sockfd);
        }
        else
        {
            printf("Timed out getting response from client!\n");
            ResetRouting(0);
            return -1;
        }
    }

    return 0;
}