#ifndef _BMP_PARSER_H
#define _BMP_PARSER_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <math.h>

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
} bitmap_info_header;

typedef union bitmap_info
{
    bitmap_core_header core;
    bitmap_info_header header;
} bitmap_info;

typedef struct rgb
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb;

typedef struct bitmap_metadata
{
    bitmap_header header;
    bitmap_info info;
    unsigned char *pixels;
} bitmap_metadata;

typedef bitmap_metadata *bitmap_metadata_ptr;

/*
 * Function: bitmap_read_metadata
 * ----------------------------
 *   Reads bitmap from file into a metadata structure, for ease of use.
 *
 *   fptr: file pointer to bitmap file.
 *
 *   returns: structure holding metadata about the bitmap.
 */
bitmap_metadata_ptr bitmap_read_metadata(FILE *fptr);

/*
 * Function: bitmap_load_pixels
 * ----------------------------
 *   Loads bitmap pixels into a char pointer. The bitmap pixels are stored in BGR format.
 *
 *   fptr: file pointer to bitmap file.
 *   file_size: size of the file.
 *   header: pointer to the bitmap header structure.
 *   info: pointer to the bitmap info structure.
 *
 *   returns: steg_configuration_ptr allocated structure.
 */
unsigned char *bitmap_load_pixels(FILE *fptr, uint64_t file_size, bitmap_header *header, bitmap_info *info);

/*
 * Function: metadata_to_file
 * ----------------------------
 *   Dumps bitmap metadata into a given file.
 *
 *   metadata: pointer to the bitmap metadata structure.
 *   file_name: path to the file to write to.
 *   returns: 0 on success, -1 on failure.
 */
int metadata_to_file(bitmap_metadata_ptr metadata, char *file_name);

#endif
