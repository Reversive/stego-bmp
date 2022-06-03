#ifndef _BMP_PARSER_H
#define _BMP_PARSER_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

#pragma pack(push, 1)

typedef struct bitmap_header
{
    union
    {
        uint16_t id;
        char header[2];
    } magic;
    uint32_t size;
    uint16_t reserved[2];
    uint32_t offset;
} bitmap_header;

typedef struct bitmap_core_header
{
    uint32_t size;
    int16_t width;
    int16_t height;
    uint16_t planes;
    uint16_t bit_count;
} bitmap_core_header;

typedef struct bitmap_info_header
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    uint32_t x_pels_per_meter;
    uint32_t y_pels_per_meter;
    uint32_t used_colors;
    uint32_t important_colors;
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t alpha_mask;
} bitmap_info_header;

typedef union bitmap_info
{
    bitmap_core_header core;
    bitmap_info_header header;
} bitmap_info;

typedef struct bitmap_metadata
{
    bitmap_header header;
    bitmap_info info;
} bitmap_metadata;

typedef bitmap_metadata *bitmap_metadata_ptr;

bitmap_metadata_ptr bitmap_read_metadata(char *bmp_file_path);

#endif