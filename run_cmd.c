#include <stdio.h>			/*printf, perror*/
#include <stdlib.h>         /*system*/
#include "vpn.h"

/*
Run command, if there is an error for that command, 
print it and return -1;
*/
int RunCMD(char *cmd)
{
    if (system(cmd) == -1)
    {
        perror(cmd);
        return -1;
    }

    return 0;
}