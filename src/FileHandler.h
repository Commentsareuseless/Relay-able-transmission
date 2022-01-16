#pragma once

#include <stdio.h>

#define FH_MODE_READ    0
#define FH_MODE_WRITE   1

#define READ_BUFF_SIZE 256

struct _FileData
{
    char* name;
    FILE* handle;
    char raedBuff[READ_BUFF_SIZE];
};
typedef struct _FileData FileData;

/**
 * @param file File name
 * @param handle returned pointer to opened file
 * @param mode FH_MODE_READ or FH_MODE_WRITE
 * @return [0] - on success
 *         [-1] - on error (inexistent file)
 */
FILE* InitFileHandler(const char* file, int mode);

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

/**
 * @brief This function is supposed to be called in loop
 *        for the same file. It writes one line at a time.
 *
 * @param data pointer to data buffer
 * @param size size of given data buffer
 * @param handle file handle to read from
 * @return [number of bytes read] - on normal operation
 *         [0] - on EoF
 *         [-1] - when error occurs
 */
int WriteTxtFile(const char* data, size_t size, FILE* handle);
