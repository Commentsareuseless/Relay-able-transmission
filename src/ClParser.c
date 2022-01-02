#include "ClParser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int PrintUsageAndFail()
{
    fprintf(stderr, "ERROR:\tInvalid arguments, usage:\n\n"
                    "\tSend file: -i [IpOfRecipient] \t-f [FileToTransfer] -p [port]\n"
                    "\tListen for files on [port]: -l [port]\n");
    return -1;
}

int ParseCommandLine(int argc, char** argv, ClArgs* out_args)
{
    if((7 != argc) && (3 != argc))
    {
        return PrintUsageAndFail();
    }

    memset(out_args, 0, sizeof(ClArgs));

    if (3 == argc)
    {
        if (0 == strncmp(argv[1], "-l", sizeof("-l")))
        {
            out_args->port = atol(argv[2]);
            out_args->typeOfService = NH_TOS_RECEIVER;
            return 0;
        }
        return PrintUsageAndFail();
    }

    for (int i = 1; i < argc; ++i)
    {
        if (0 == strncmp(argv[i], "-i", sizeof("-i")))
        {
            memcpy(out_args->ipv4Address, argv[++i], sizeof(out_args->ipv4Address));
            continue;
        }

        if (0 == strncmp(argv[i], "-f", sizeof("-f")))
        {
            memcpy(out_args->fileName, argv[++i], sizeof(out_args->fileName));
            continue;
        }

        if (0 == strncmp(argv[i], "-p", sizeof("-p")))
        {
            out_args->port = atol(argv[++i]);
            continue;
        }
        return PrintUsageAndFail();
    }
    out_args->typeOfService = NH_TOS_TRANSMITTER;

    return 0;
}

