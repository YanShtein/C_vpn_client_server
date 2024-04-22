#include <stdio.h>			/*printf*/
#include <time.h>           /*time struct*/
#include "vpn.h"


void Log()
{
    time_t rawtime;
    struct tm *timeinfo;
    char time_output[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_output, sizeof(time_output), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("%s ", time_output);
}