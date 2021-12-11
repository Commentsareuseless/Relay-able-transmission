#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "FileReader.h"

/*
1. Przeczytać argumenty z cl
    a. IP odbiorcy
    b. plik do wysłania
    Żeby ciężej było zepsuć, ip będzie poprzedzone opcją -ip a plik -f


*/

static FILE* fileToRead;
static unsigned recPort = 5050;
static struct sockaddr_in addressOfRecipient;

static void ParseCommandLine(int argc, char** argv)
{
    if(5 != argc)
    {
        fprintf(stderr, "ERROR:\t Invalid arguments\n"
                        "Usage: -i IpOfRecipient -f FileToTransfer\n");
        return;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-i", sizeof("-i")))
        {
            
        }

        if (strncmp(argv[i], "-f", sizeof("-f")))
        {
            
        }

    }

    addressOfRecipient.sin_family       = AF_INET; // IPv4
    addressOfRecipient.sin_addr.s_addr  = INADDR_ANY;
    addressOfRecipient.sin_port         = htons(recPort);
}


int main(int argc, char** argv)
{
    FILE* fileToRead = NULL;
    struct sockaddr_in recAddr;


    ParseCommandLine(argc, argv)
    InitFileReader("TegoPlikuNieMa", fileToRead);

    printf("Dziala lul\n");
    return 0;
}
