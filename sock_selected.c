#include <unistd.h>         /*read*/
#include <strings.h>        /*bzero*/
#include <stdio.h>			/*printf, perror*/
#include "vpn.h"

#define MAX_LEN 1400

/*
Reads data from socket and back to tun interface
*/
void SockSelected(int tunfd, int sockfd)
{
    char buff[MAX_LEN] = {0};
    int len;

    printf("Got packet from socket\n");

    bzero(buff, MAX_LEN);
    /*init buffer and receive data from socket into it*/
    len = recvfrom(sockfd, buff, MAX_LEN, 0, NULL, NULL);
    /*write bytes from buff to tun interface*/
    write(tunfd, buff, len);
}