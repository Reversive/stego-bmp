// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./include/file.h"

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot;
}

uint32_t get_file_size(FILE *fp)
{
    // Go to end
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    // Reset ptr
    fseek(fp, 0L, SEEK_SET);
    return size;
}

void copy_file_content(FILE *fp, char *buff)
{
    int c;
    for (int i = 0; EOF != (c = fgetc(fp)); i++)
        buff[i] = c;
}
