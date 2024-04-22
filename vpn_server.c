#include <stdio.h>			/*printf, perror*/
#include <stdlib.h>         /*free, NULL, system*/
#include "vpn.h"

#define SERVER_PORT 4500
#define TUN_DEVICE "/dev/net/tun"

int main(void)
{
    vpn_socket_handle_t *vpn_sock_handle = Init(SERVER_PORT);

    if (vpn_sock_handle == NULL)
    {
        return 1;
    }

    Log();
    printf("VPN server Initialized\n");
    Log();
    printf("UDP: server port %i\n",
        ntohs(vpn_sock_handle->vpn_sock_addr_bind.sin_port));

    vpn_sock_handle->tunfd = CreateTunDev(TUN_DEVICE);
    if (vpn_sock_handle->tunfd < 0)
    {
        CleanUp(vpn_sock_handle);
        printf("Resources freed\n");
        return -1;
    }

    Ifconfig(0);

    if (ServerConnection(vpn_sock_handle) < 0)
    {
        CleanUp(vpn_sock_handle);
        printf("Resources freed\n");
        return -1;
    }

    CleanUp(vpn_sock_handle);

    return 0;
}