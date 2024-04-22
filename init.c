#include <stdio.h>			/*printf, perror*/
#include <stdlib.h>         /*NULL*/
#include "vpn.h"

vpn_socket_handle_t *Init(int port)
{
    vpn_socket_handle_t *vpn_sock_handle = (vpn_socket_handle_t*)malloc(sizeof(vpn_socket_handle_t));

    vpn_sock_handle->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (vpn_sock_handle->sockfd == -1)
    {
        perror("init (socket)");
        free(vpn_sock_handle);
        return NULL;
    }

    vpn_sock_handle->vpn_sock_addrlen = sizeof(vpn_sock_handle->vpn_sock_addr_bind);
    vpn_sock_handle->vpn_sock_addr_bind.sin_family = AF_INET;
    vpn_sock_handle->vpn_sock_addr_bind.sin_port = htons(port);
    vpn_sock_handle->vpn_sock_addr_bind.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(vpn_sock_handle->sockfd, (struct sockaddr*)&vpn_sock_handle->vpn_sock_addr_bind, 
        vpn_sock_handle->vpn_sock_addrlen) != 0)
    {
        perror("init (bind)");
        free(vpn_sock_handle);
        return NULL;
    }

    return vpn_sock_handle;
}