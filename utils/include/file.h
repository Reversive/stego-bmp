#ifndef _FILE_H
#define _FILE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
const char *get_filename_ext(const char *filename);
uint32_t get_file_size(FILE *fp);
void copy_file_content(FILE *fp, char *buff);
#endif
