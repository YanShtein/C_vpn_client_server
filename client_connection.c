#include <stdio.h>			/*printf, perror*/
#include <sys/ioctl.h>		/*for socket configs*/
#include <sys/select.h>		/*listen for socket activity*/
#include <string.h>			/*memset*/
#include <unistd.h>         /*read*/
#include <strings.h>        /*bzero*/
#include <signal.h>		    /*sig_atomic*/
#include "vpn.h"

#define MAX_MSG_LEN 1400
#define TUN_DEVICE "/dev/net/tun"
#define CLIENT_MESSAGE "Hello from client\n"
#define SRV_EXPECTED_MESSAGE "Hello from server\n"
#define CLIENT_MESSAGE_WAKE "Boop"

static sig_atomic_t g_run = 1;
/*used when pressed ctrl^c to send interrupt signal
if keyboard interrupted with signal, select() sets flag EINTR with the number 
of the return value in select() and returns back to Init().*/
void KeyboardSignalHandler()
{
    g_run = 0;
    ResetRouting(1);
}

/*Client tries connecting to server, if timeout, client exits.
else if client received hello it will proceed initiating tun0 and configurations.*/
int ClientConnection(vpn_socket_handle_t *vpn_sock_handle)
{
    struct timeval timeout;
    fd_set readfds;
    char server_message[MAX_MSG_LEN] = {0};
    struct sockaddr_in server_addr = {0};
    socklen_t server_addrlen = sizeof(server_addr);

    signal(SIGINT, KeyboardSignalHandler);

    /*convert from host byte order to network byte order*/
    server_addr.sin_port = htons(4500);
    server_addr.sin_addr.s_addr = inet_addr("192.168.57.2");

    if (sendto(vpn_sock_handle->sockfd, CLIENT_MESSAGE, sizeof(CLIENT_MESSAGE), 0,
    (struct sockaddr*)&server_addr, server_addrlen) < 0)
    {
        perror("handle_conn (send)");
        return -1;
    }

    /* timeout for response 2.5 seconds */
    timeout.tv_sec = 2;
    timeout.tv_usec = 500000;
    /* clear entries from set */
    FD_ZERO(&readfds);
    FD_SET(vpn_sock_handle->sockfd, &readfds);

    /*
    watch fds related to sockfd, until there is data to read or timeout.
    */
    select(vpn_sock_handle->sockfd + 1, &readfds, NULL, NULL, &timeout);

    /*
    if fd is ready to read than recv from, else, fd was timed out.
    */
    if (FD_ISSET(vpn_sock_handle->sockfd, &readfds))
    {
        /*receive from server*/
        if (recvfrom(vpn_sock_handle->sockfd, server_message, sizeof(server_message), 0,
        (struct sockaddr*)&server_addr, &server_addrlen) < 0)
        {
            perror("handle_conn (recv)");
            return -1;
        }

        if (strcmp(server_message, SRV_EXPECTED_MESSAGE) != 0)
        {
            printf("Unexpected message from server: %s! exiting...", server_message);
            return -1;
        }
    }
    else
    {
        printf("Timed out trying to connect\n");
        return -1;
    }

    vpn_sock_handle->tunfd = CreateTunDev(TUN_DEVICE);
    if (vpn_sock_handle->tunfd < 0)
    {
        return -1;
    }

    Ifconfig(1);

    Log();
    printf("Received from peer: %s:%i\n",
        inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    printf("Message from server: %s", server_message);
    printf("process running...\n");

    SetRouting(1);

    while (g_run) 
    {
        fd_set readFDSet;
        /*maximum file descriptor value between tunfd and sockfd*/
        int maxfd = (vpn_sock_handle->tunfd > vpn_sock_handle->sockfd) ? 
        vpn_sock_handle->tunfd : vpn_sock_handle->sockfd;

        timeout.tv_sec = 8;
        timeout.tv_usec = 0;

        /*clear the set before adding file descriptors*/
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
            TunSelected(vpn_sock_handle->tunfd, vpn_sock_handle->sockfd, &server_addr);
        }
        else if (FD_ISSET(vpn_sock_handle->sockfd, &readFDSet))
        {
            SockSelected(vpn_sock_handle->tunfd, vpn_sock_handle->sockfd);
        }
        else
        {
            printf("Timed out getting response from server!\n");
            ResetRouting(1);
            return -1;
        }
    }

    return 0;
}