#include <stdio.h>          /*printf*/
#include <linux/if.h>       /*ifreq*/
#include <linux/if_tun.h>	/*IFF_TUN*/
#include <sys/ioctl.h>      /*ioctl*/
#include <fcntl.h>          /*open, close*/
#include <string.h>			/*memset*/
#include <unistd.h>         /*close*/
#include "vpn.h"

int CreateTunDev(char *dev)
{
    /*
    used to configure the network interface, part of the Linux networking API.
    */
    struct ifreq ifr;
    int fd;

    if ((fd = open(dev, O_RDWR)) < 0)
    {
        perror("create_tun (fd)");
        return 1;
    }

    /* initializes structure with zero */
    memset(&ifr, 0, sizeof(ifr));
    /* sets interface flags to indicate it's a TUN device (no packet information) */
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    /*set name of device*/
    strncpy(ifr.ifr_name, "tun0", IFNAMSIZ);

    /*system call ioctl uses TUNSETIFF to set attributes of network interace
    associated with fd*/
    if (ioctl(fd, TUNSETIFF, (void*)&ifr) < 0)
    {
        perror("create_tun (ioctl)");
        close(fd);
        return 1;
    }

    Log();
    printf("TUN/TAP device tun0 opened\n");

    return fd;
}