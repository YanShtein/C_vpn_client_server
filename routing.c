#include "vpn.h"

/*
Setup routing table
host type: (1) for client, (0) for server
*/
void SetRouting(int is_client)
{
    /* system("sysctl -w net.ipv4.ip_forward=1"); */
    if (is_client)
    {
        /*forward all traffic through tun0*/
        RunCMD("ip route add 0.0.0.0/1 dev tun0");
        RunCMD("ip route add 128.0.0.0/1 dev tun0");
        /*specific route to server after packet was encapsulated again, with its 
        external address*/
        RunCMD("ip route add 192.168.57.2 via 192.168.56.1");
    }
    else
    {
        /*specific route to client*/
        RunCMD("ip route add 192.168.56.2 via 192.168.57.1");
        /*enable NAT for traffic from vpn subnet not destined for vpn subnet
        out external*/
        RunCMD("iptables -t nat -A POSTROUTING -s 172.16.1.0/30 ! -d 172.16.1.0/30 -o enp0s3 -j MASQUERADE");
    }
}

void ResetRouting(int is_client)
{
    /* system("sysctl -w net.ipv4.ip_forward=1"); */
    if (is_client)
    {
        RunCMD("ip route del 0.0.0.0/1 dev tun0");
        RunCMD("ip route del 128.0.0.0/1 dev tun0");
        RunCMD("ip route del 192.168.57.2 via 192.168.56.1");
    }
    else
    {
        RunCMD("ip route del 192.168.56.2 via 192.168.57.1");
        RunCMD("iptables -t nat -D POSTROUTING -s 172.16.1.0/30 ! -d 172.16.1.0/30 -o enp0s3 -j MASQUERADE");
    }
}