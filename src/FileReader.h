#pragma once

#include <stdio.h>

int InitFileReader(const char* file, FILE* handle);

int ReadTxtFile(char* out_data, size_t size, FILE* handle);
