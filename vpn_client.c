#include <stdio.h>			/*printf, perror*/
#include <stdlib.h>         /*NULL*/
#include "vpn.h"

#define CLIENT_PORT 4000

int main(void)
{
    vpn_socket_handle_t *vpn_sock_handle = Init(CLIENT_PORT);

    if (vpn_sock_handle == NULL)
    {
        return -1;
    }

    Log();
    printf("VPN client Initialized\n");
    Log();
    printf("UDP: client port %i\n",
        ntohs(vpn_sock_handle->vpn_sock_addr_bind.sin_port));

    if (ClientConnection(vpn_sock_handle) < 0)
    {
        CleanUp(vpn_sock_handle);
        printf("Resources freed\n");
        return -1;
    }

    CleanUp(vpn_sock_handle);

    return 0;
}