#pragma once

#include "NetHandler.h"

struct _ClArgs
{
    char ipv4Address[16];
    long port;
    char fileName[32];
    int typeOfService;
};
typedef struct _ClArgs ClArgs;

/**
 * @param argc number of CL arguments
 * @param argv array of arguments as strings
 * @param out_args outputted struct of parsed args
 * @return [0] - on success [-1] - when args are invalid
 */
int ParseCommandLine(int argc, char** argv, ClArgs* out_args);
