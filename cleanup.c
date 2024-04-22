#include <stdlib.h>         /*for malloc and free*/
#include <unistd.h>         /*for closing file descriptor*/
#include "vpn.h"

/*receives struct handle, releases file descriptors and free handle*/
void CleanUp(vpn_socket_handle_t *vpn_sock_handle)
{
    close(vpn_sock_handle->sockfd);
    close(vpn_sock_handle->tunfd);
    free(vpn_sock_handle);
}