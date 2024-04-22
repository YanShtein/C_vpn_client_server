#include <unistd.h>         /*read*/
#include <strings.h>        /*bzero*/
#include <stdio.h>			/*printf, perror*/
#include "vpn.h"

#define MAX_LEN 1400

/*
Reads data from tun and sends packet to peer
*/
void TunSelected(int tunfd, int sockfd, struct sockaddr_in *peer)
{
    char buff[MAX_LEN] = {0};
    int len;

    printf("Got packet from TUN\n");
    /*init buffer*/
    bzero(buff, MAX_LEN);
    /*read data from tun to buffer*/
    len = read(tunfd, buff, MAX_LEN);
    /*send the packet to peer using socket*/
    sendto(sockfd, buff, len, 0, (struct sockaddr*)peer, sizeof(*peer));
}