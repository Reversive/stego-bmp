// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/entry_point.h"

int main(
    int argc,
    char *argv[])
{
    steg_configuration_ptr steg_config = parse_options(argc, argv);

    FILE *carrier_fptr = fopen(steg_config->bmp_carrier_path, "rw");
    if (carrier_fptr == NULL)
    {
        logw(ERROR, "%s\n", "Invalid carrier file path.");
        exit_clean_up(STATUS_ERROR, NULL, steg_config, NULL);
    }

    bitmap_metadata_ptr bmp_metadata = bitmap_read_metadata(carrier_fptr);
    if (bmp_metadata == NULL)
    {
        logw(ERROR, "%s\n", "Couldn't parse bmp carrier.");
        exit_clean_up(STATUS_ERROR, carrier_fptr, steg_config, NULL);
    }

    password_data p_data = {0};
    int should_encrypt = steg_config->enc_password != NULL;
    if (should_encrypt)
    {
        p_data.password = steg_config->enc_password;
        if (-1 == init_password_data(&p_data, steg_config->algo_mode, steg_config->block_mode))
        {
            logw(ERROR, "%s\n", "Couldn't init password data.");
            exit_clean_up(STATUS_ERROR, carrier_fptr, steg_config, bmp_metadata);
        }
    }

    char *payload = NULL;
    if (steg_config->action == EMBED)
    {
        size_t payload_size;
        payload = generate_payload(steg_config, &payload_size, p_data, should_encrypt);
        if (payload == NULL)
        {
            exit_clean_up(ERROR, carrier_fptr, steg_config, bmp_metadata);
        }
        logw(DEBUG, "%s\n", "Hiding payload into meta");
        if (-1 == hide_payload_into_meta(steg_config->steg_mode, payload, bmp_metadata, payload_size))
        {
            logw(ERROR, "%s", "Error hiding payload");
            exit_clean_up(STATUS_ERROR, carrier_fptr, steg_config, bmp_metadata);
        }
        if (-1 == metadata_to_file(bmp_metadata, steg_config->bmp_out_path))
        {
            logw(ERROR, "Error dumping meta in file %s", steg_config->bmp_out_path);
            exit_clean_up(STATUS_ERROR, carrier_fptr, steg_config, bmp_metadata);
        }
    }
    else
    {
        unsigned char *extracted_payload;
        logw(DEBUG, "%s\n", "Extracting payload from meta");
        extracted_payload = extract_payload_from_meta(steg_config->steg_mode, bmp_metadata, should_encrypt);
        if (-1 == unload_payload(extracted_payload, &p_data, should_encrypt, steg_config->bmp_out_path))
        {
            logw(ERROR, "Error unloading payload in %s", steg_config->bmp_out_path);
            exit_clean_up(STATUS_ERROR, carrier_fptr, steg_config, bmp_metadata);
        }
        free(extracted_payload);
    }

    if (payload)
    {
        free(payload);
    }
    if (should_encrypt)
    {
        clear_password_data(&p_data);
    }
    exit_clean_up(STATUS_SUCCESS, carrier_fptr, steg_config, bmp_metadata);
}

void exit_clean_up(
    int err_code,
    FILE *carrier_fptr,
    steg_configuration_ptr steg_config,
    bitmap_metadata_ptr bmp_metadata)
{
    if (steg_config != NULL)
    {
        free(steg_config);
    }
    if (bmp_metadata != NULL)
    {
        if (bmp_metadata->pixels != NULL)
        {
            free(bmp_metadata->pixels);
        }
        free(bmp_metadata);
    }
    if (carrier_fptr != NULL)
    {
        fclose(carrier_fptr);
    }
    exit(err_code);
}
