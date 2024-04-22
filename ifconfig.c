#include <stdio.h>			/*printf*/
#include <string.h>			/*memset*/
#include "vpn.h"

#define MAX_LEN 1024
/*
configure IP address and MTU of VPN interface tun0
host type: (1) for client, (0) for server
*/
int Ifconfig(int host_type)
{
    char cmd[MAX_LEN];

    memset(cmd, '\0', sizeof(cmd));

    if (host_type == 1)
    {
        sprintf(cmd, "%s", "ip link set tun0 mtu 1400 up");
        RunCMD(cmd);
        Log();
        printf("net_addr_v4_up: %s\n", cmd);

        sprintf(cmd, "%s", "ip address add 172.16.1.1/30 dev tun0");
        RunCMD(cmd);
        Log();
        printf("net_addr_v4_add: %s\n", cmd);
    }
    else
    {
        sprintf(cmd, "%s", "ip link set tun0 mtu 1400 up");
        RunCMD(cmd);
        Log();
        printf("net_addr_v4_up: %s\n", cmd);

        sprintf(cmd, "%s", "ip address add 172.16.1.2/30 dev tun0");
        RunCMD(cmd);
        Log();
        printf("net_addr_v4_add: %s\n", cmd);
    }
    
    return 0;
}