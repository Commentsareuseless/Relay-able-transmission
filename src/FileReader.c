#include "FileReader.h"

int InitFileReader(const char* file, FILE* handle)
{
    int ret = 0;
    FILE* fileHandle = NULL;
    fileHandle = fopen(file, "r+");

    if(NULL == fileHandle)
    {
        fprintf(stderr, "ERROR:\t Could not open file, (does not exist)\n");
        return 1;
    }
    return 0;
}

int ReadTxtFile(char* out_data, size_t size, FILE* handle)
{
    size_t bytesRead = 0;

    if(NULL == handle)
    {
        fprintf(stderr, "ERROR:\t Invalid file handle!!!\n");
        return 1;
    }

    bytesRead = fread(out_data, 1, size, handle);

    if (bytesRead != size)
    {
        if(feof(handle))
        {
            // Osiągnęliśmy koniec pliku, jest git
            return 0;
        }
        // Jakiś błąd, nie fajnie :(
        fprintf(stderr, "ERROR:\t Sth went wrong during reading of file :(\n");
        return 1;
    }
}
