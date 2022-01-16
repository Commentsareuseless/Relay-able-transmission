#include "FileHandler.h"

FILE* InitFileHandler(const char* file, int mode)
{
    FILE* handle = NULL;

    switch (mode)
    {
    case FH_MODE_READ:
        handle = fopen(file, "r+");
        break;

    case FH_MODE_WRITE:
        handle = fopen(file, "w+");
        break;
    default:
        fprintf(stderr, "ERROR:\t Unsupported mode\n");
        return NULL;
    }

    if(NULL == handle)
    {
        fprintf(stderr, "ERROR:\t Could not initialize file\n");
        return NULL;
    }

    return handle;
}

int ReadTxtFile(char* out_data, size_t size, FILE* handle)
{
    size_t bytesRead = 0;

    if(NULL == handle)
    {
        fprintf(stderr, "ERROR:\t Invalid file handle!!!\n");
        return -1;
    }

    bytesRead = fread(out_data, 1, size, handle);

    if (bytesRead != size)
    {
        if(feof(handle))
        {
            // Osiągnęliśmy koniec pliku, jest git
            return bytesRead;
        }
        // Jakiś błąd, nie fajnie :(
        fprintf(stderr, "ERROR:\t Sth went wrong during reading of a file :(\n");
        return -1;
    }
    return bytesRead;
}

int WriteTxtFile(const char* data, size_t size, FILE* handle)
{
    size_t bytesWritten = 0;

    if(NULL == handle)
    {
        fprintf(stderr, "ERROR:\t Invalid file handle!!!\n");
        return -1;
    }

    bytesWritten = fwrite(data, sizeof(char), size, handle);

    if (bytesWritten != size)
    {
        fprintf(stderr, "ERROR:\t Sth went wrong during writting to file :(\n");
        return -1;
    }
    return bytesWritten;
}
