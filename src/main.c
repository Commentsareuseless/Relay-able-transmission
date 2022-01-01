#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "FileHandler.h"
#include "ClParser.h"

/*
1. Przeczytać argumenty z cl
    a. IP odbiorcy
    b. plik do wysłania
    Żeby ciężej było zepsuć, ip będzie poprzedzone opcją -i a plik -f


*/

static ClArgs clArgs = {0};
static FileData fd = {0};
static unsigned recPort = 5050;
static struct sockaddr_in addressOfRecipient;

int main(int argc, char** argv)
{
    struct sockaddr_in recAddr;

    if (0 > ParseCommandLine(argc, argv, &clArgs))
    {
        exit(1);
    }

    switch (clArgs.typeOfService)
    {
    case SEND:
        InitFileHandler(clArgs.fileName, fd.handle, FH_MODE_READ);
        // Send file
        break;

    case RECEIVE:
        // Wait for connection (First received string is file name)
        memcpy(clArgs.fileName, "plick.txt", sizeof("plick.txt"));
        InitFileHandler(clArgs.fileName, fd.handle, FH_MODE_WRITE);
        break;
    default:
        fprintf(stdout, "Sth went wrong :(, invalid type of service\n");
        return -1;
    }

    // Cleanup

    printf("Dziala lul\n");
    return 0;
}
