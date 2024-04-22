/*
	Developer: Yana Brushtein
	Date: 
	Reviewer: 
	Status: 
	Version: 
*/

#ifndef __CR6_vpn_H__
#define __CR6_vpn_H__

#include <arpa/inet.h>      /*sockaddr_in*/

typedef struct vpn_socket_handle
{
    int sockfd;
    int tunfd;
    int vpn_sock_addrlen;
    struct sockaddr_in vpn_sock_addr_bind;
} vpn_socket_handle_t;


vpn_socket_handle_t *Init(int port);
void CleanUp(vpn_socket_handle_t *vpn_sock_handle);
int CreateTunDev(char *dev);
int ServerConnection(vpn_socket_handle_t *vpn_sock_handle);
int ClientConnection(vpn_socket_handle_t *vpn_sock_handle);
void Log();
int Ifconfig(int host_type);
void SetRouting(int host_type);
void ResetRouting(int host_type);
int RunCMD(char *cmd);
void SockSelected(int tunfd, int sockfd);
void TunSelected(int tunfd, int sockfd, struct sockaddr_in *peer);

#endif /* __CR6_vpn_H__ */