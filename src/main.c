#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include "ClParser.h"
#include "NetHandler.h"


int main(int argc, char** argv)
{
    long elapsedTime = 0;;
    ClArgs clArgs = {0};
    struct timeval start = {0}, stop = {0};

    gettimeofday(&start, NULL);

    if (0 > ParseCommandLine(argc, argv, &clArgs))
    {
        exit(1);
    }

    InitNetHandler(clArgs.typeOfService, clArgs.ipv4Address, clArgs.port);

    switch (clArgs.typeOfService)
    {
    case NH_TOS_TRANSMITTER:
        if (0 > SendFile(clArgs.fileName)) {return -1;}
        break;

    case NH_TOS_RECEIVER:
        if(0 > WaitForTransmission()) {return -1;}
        break;
    }

    gettimeofday(&stop, NULL);

    elapsedTime = (stop.tv_sec * 1000 + stop.tv_usec / 1000) -
        (start.tv_sec * 1000 + start.tv_usec / 1000);

    printf( "=============================="
            "\tFinished in: %ld ms\n", elapsedTime);
    return 0;
}
