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
    if (in_fptr != NULL)
    {
        fclose(in_fptr);
    }
    exit(err_code);
}

char *generate_payload(const char *in_file_path, size_t *payload_size)
{
    uint32_t file_size = get_file_size(in_fptr);
    const char *ext = get_filename_ext(in_file_path);
    *payload_size = sizeof(uint32_t) + file_size + strlen(ext) + 1;
    char *buff = calloc(*payload_size, sizeof(char));
    uint32_t be_file_size = htonl(file_size);
    memcpy(buff, &be_file_size, sizeof(uint32_t));
    copy_file_content(in_fptr, buff + sizeof(uint32_t));
    memcpy(buff + file_size + sizeof(uint32_t), ext, strlen(ext));
    return buff;
}

int main(
    int argc,
    char *argv[])
{
    steg_config = parse_options(argc, argv);
    carrier_fptr = fopen(steg_config->bmp_carrier_path, "rw");
    if (carrier_fptr == NULL)
    {
        logw(ERROR, "%s\n", "Invalid carrier file path.");
        exit_clean_up(STATUS_ERROR);
    }
    bmp_metadata = bitmap_read_metadata(carrier_fptr);
    if (bmp_metadata == NULL)
    {
        exit_clean_up(STATUS_ERROR);
    }

    for (int i = 0; i < bmp_metadata->info.header.height * bmp_metadata->info.header.width * 3; i += 3)
    {
        printf("R: %d G: %d B: %d\n", bmp_metadata->pixels[i], bmp_metadata->pixels[i + 1], bmp_metadata->pixels[i + 2]);
    }

    in_fptr = fopen(steg_config->in_file_path, "rw");
    if (in_fptr == NULL)
    {
        logw(ERROR, "%s\n", "Invalid in file path.");
        exit_clean_up(STATUS_ERROR);
    }

    size_t payload_size;
    char *payload = generate_payload(steg_config->in_file_path, &payload_size);
    for (size_t i = 0; i < payload_size; i++)
    {
        printf("\\%02hhx", (unsigned char)payload[i]);
    }

    password_data p_data;
    if (steg_config->enc_password != NULL)
    {
        p_data.password = steg_config->enc_password;
        init_password_data(&p_data, steg_config->algo_mode, steg_config->block_mode);
    }

    free(payload);
    exit_clean_up(STATUS_SUCCESS);
}
