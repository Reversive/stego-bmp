#include "include/bmp_parser.h"

bitmap_metadata_ptr bitmap_read_metadata(char *bmp_file_path)
{
    bitmap_metadata *metadata = malloc(sizeof(bitmap_metadata));
    if (metadata == NULL)
    {
        log(ERROR, "%s\n", "Insufficient memory to allocate bitmap metadata.");
        return NULL;
    }
    FILE *fptr = fopen(bmp_file_path, "r");
    if (fptr == NULL)
    {
        log(ERROR, "%s\n", "Invalid carrier file path.");
        free(metadata);
        return NULL;
    }

    size_t read_header = fread(&metadata->header, sizeof(bitmap_header), 1, fptr);
    if (!read_header)
    {
        log(ERROR, "%s\n", "Failure reading bitmap header.");
        free(metadata);
        fclose(fptr);
        return NULL;
    }

    size_t read_core = fread(&metadata->info, sizeof(bitmap_info), 1, fptr);
    if (!read_core)
    {
        log(ERROR, "%s\n", "Failure reading bitmap core.");
        free(metadata);
        fclose(fptr);
        return NULL;
    }

    return metadata;
}