#pragma once

#include <stdio.h>

struct _FileData
{
    char* name;
    FILE* handle;
    char raedBuff[256];
};
typedef struct _FileData FileData;

/**
 * @param file File name
 * @param handle returned pointer to opened file
 * @return [0] - on success
 *         [-1] - on error (inexistent file)
 */
int InitFileReader(const char* file, FILE* handle);

/**
 * @brief This function is supposed to be called in loop
 *        for the same file. It reads one line at a time.
 *
 * @param out_data pointer to data buffer
 * @param size size of given data buffer
 * @param handle file handle to read from
 * @return [number of bytes read] - on normal operation
 *         [0] - on EoF
 *         [-1] - when error occurs
 */
int ReadTxtFile(char* out_data, size_t size, FILE* handle);
