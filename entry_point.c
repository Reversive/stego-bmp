#include "include/entry_point.h"

FILE *carrier_fptr, *in_fptr;
steg_configuration_ptr steg_config;
bitmap_metadata_ptr bmp_metadata;

void exit_clean_up(int err_code)
{
    if (steg_config != NULL)
    {
        free(steg_config);
        steg_config = NULL;
    }
    if (bmp_metadata != NULL)
    {
        free(bmp_metadata);
        bmp_metadata = NULL;
    }
    if (carrier_fptr != NULL)
    {
        fclose(carrier_fptr);
    }
    exit(err_code);
}

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot;
}

const uint32_t get_file_size(FILE *fp)
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
    char c;
    for (int i = 0; EOF != (c = fgetc(fp)); i++)
        buff[i] = c;
}
int main(
    int argc,
    char *argv[])
{
    steg_config = parse_options(argc, argv);
    carrier_fptr = fopen(steg_config->bmp_carrier_path, "rw");
    if (carrier_fptr == NULL)
    {
        log(ERROR, "%s\n", "Invalid carrier file path.");
        exit_clean_up(STATUS_ERROR);
    }
    bmp_metadata = bitmap_read_metadata(carrier_fptr);
    if (bmp_metadata == NULL)
    {
        exit_clean_up(STATUS_ERROR);
    }

    in_fptr = fopen(steg_config->in_file_path, "rw");
    if (in_fptr == NULL)
    {
        log(ERROR, "%s\n", "Invalid in file path.");
        exit_clean_up(STATUS_ERROR);
    }

    uint32_t file_size = get_file_size(in_fptr);
    const char *ext = get_filename_ext(steg_config->in_file_path);
    char *buff = calloc(4 + file_size + strlen(ext) + 1, sizeof(char));
    memcpy(buff, &file_size, 4);
    copy_file_content(in_fptr, buff + 4);
    memcpy(buff + file_size + 4, ext, strlen(ext));
    printf("%s", buff + 4);

    exit_clean_up(STATUS_SUCCESS);
}
