// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/bmp_parser.h"
#include "include/file.h"

bitmap_metadata_ptr bitmap_read_metadata(FILE *fptr)
{
    bitmap_metadata *metadata = malloc(sizeof(bitmap_metadata));
    if (metadata == NULL)
    {
        logw(ERROR, "%s\n", "Insufficient memory to allocate bitmap metadata.");
        return NULL;
    }
    size_t read_header = fread(&metadata->header, sizeof(bitmap_header), 1, fptr);
    if (!read_header)
    {
        logw(ERROR, "%s\n", "Failure reading bitmap header.");
        free(metadata);
        fclose(fptr);
        return NULL;
    }

    size_t read_core = fread(&metadata->info, sizeof(bitmap_info), 1, fptr);
    if (!read_core)
    {
        logw(ERROR, "%s\n", "Failure reading bitmap core.");
        free(metadata);
        fclose(fptr);
        return NULL;
    }

    uint64_t current_pos = ftell(fptr);
    fseek(fptr, 0l, SEEK_END);
    uint64_t file_size = ftell(fptr) - current_pos;
    fseek(fptr, current_pos, SEEK_SET);

    metadata->pixels = bitmap_load_pixels(fptr, file_size, &metadata->header, &metadata->info);

    if (metadata->pixels == NULL)
    {
        return NULL;
    }

    return metadata;
}

unsigned char *bitmap_load_pixels(
    FILE *fptr,
    uint64_t file_size,
    bitmap_header *header,
    bitmap_info *info)
{
    bitmap_info_header *core = &info->header;

    if (file_size == 0)
    {
        logw(ERROR, "%s\n", "No pixel data in file\n");
        return NULL;
    }

    if (core->width < 0)
    {
        core->width = abs(core->width);
    }

    if (!core->width || !core->height)
    {
        logw(ERROR, "%s\n", "Invalid image height/width.");
        return NULL;
    }

    uint64_t pixels_needed = ((uint64_t)core->width) * ((uint64_t)abs(core->height)) * sizeof(rgb);
    if (pixels_needed > 0xFFFFFFFF)
    {
        logw(ERROR, "%s\n", "Number of pixels can't be larger than what can be stored in a uint32_t.");
        return NULL;
    }

    if (core->compression != 0)
    {
        logw(ERROR, "%s\n", "Compressed images are not supported.");
        return NULL;
    }

    if (core->bit_count != 24)
    {
        logw(ERROR, "%s\n", "The image does not contain 24 bits per pixel.");
        return NULL;
    }

    if (header->offset && header->offset < file_size)
    {
        fseek(fptr, header->offset, SEEK_SET);
    }

    int row_padded = (core->width * 3 + 3) & (~3);
    unsigned char *raw_data = malloc(row_padded);
    if (raw_data == NULL)
    {
        logw(ERROR, "%s\n", "Insufficient memory to allocate raw data.");
        return NULL;
    }
    unsigned char *data = malloc(3 * core->width * core->height);
    if (data == NULL)
    {
        logw(ERROR, "%s\n", "Insufficient memory to allocate data.");
        free(raw_data);
        return NULL;
    }

    for (int i = 0; i < core->height; i++)
    {
        fread(raw_data, sizeof(unsigned char), row_padded, fptr);
        for (int j = 0; j < core->width * 3; j += 3)
        {
            int start = i * core->width * 3 + j;
            data[start] = raw_data[j];
            data[start + 1] = raw_data[j + 1];
            data[start + 2] = raw_data[j + 2];
        }
    }
    free(raw_data);

    return data;
}

int metadata_to_file(bitmap_metadata_ptr metadata, char *file_name)
{
    if (strcmp(get_filename_ext(file_name), ".bmp") != 0)
    {
        logw(ERROR, "File of extension %s is not a bmp file\n", get_filename_ext(file_name));
        return -1;
    }
    FILE *fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        logw(ERROR, "%s\n", "Can't create file to insert metadata.");
        return -1;
    }
    if (fwrite(&metadata->header, sizeof(bitmap_header), 1, fp) != 1)
    {
        logw(ERROR, "%s\n", "Can't write header to file.");
        fclose(fp);
        return -1;
    }
    if (fwrite(&metadata->info, sizeof(bitmap_info), 1, fp) != 1)
    {
        logw(ERROR, "%s\n", "Can't write core to file.");
        fclose(fp);
        return -1;
    }
    size_t pixel_count = metadata->info.header.width * metadata->info.header.height;
    if (fwrite(metadata->pixels, sizeof(rgb), pixel_count, fp) != pixel_count)
    {
        logw(ERROR, "%s\n", "Can't write pixels to file.");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}