// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/entry_point.h"
#include "./include/lsb.h"
#include <stdio.h>
#include <stdlib.h>

FILE *carrier_fptr;
steg_configuration_ptr steg_config;
bitmap_metadata_ptr bmp_metadata;

static void sigterm_handler(const int signal);
void exit_clean_up(int err_code);
// Generates payload from file without encryption
char *generate_raw_payload(const char *in_file_path, size_t *raw_payload_size);
// Generates payload from file with (if necessary) encryption
char *generate_payload(const char *in_file_path, size_t *payload_size, password_data p_data, int should_encrypt);
// Dumps pre-generated payload into file
int unload_payload(unsigned char *payload, password_data *p_data, int should_encrypt, char *out_file_name);

int main(
    int argc,
    char *argv[])
{
    // Signal config
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);

    steg_config = parse_options(argc, argv);

    // Carrier is mandatory for any option
    carrier_fptr = fopen(steg_config->bmp_carrier_path, "rw");
    if (carrier_fptr == NULL)
    {
        logw(ERROR, "%s\n", "Invalid carrier file path.");
        exit_clean_up(STATUS_ERROR);
    }

    // Parse bitmap
    bmp_metadata = bitmap_read_metadata(carrier_fptr);
    if (bmp_metadata == NULL)
    {
        logw(ERROR, "%s\n", "Couldn't parse bmp carrier.");
        exit_clean_up(STATUS_ERROR);
    }

    password_data p_data = {0};
    int should_encrypt = steg_config->enc_password != NULL;

    // Init encryption data
    if (should_encrypt)
    {
        p_data.password = steg_config->enc_password;
        if (-1 == init_password_data(&p_data, steg_config->algo_mode, steg_config->block_mode))
        {
            logw(ERROR, "%s\n", "Couldn't init password data.");
            exit_clean_up(STATUS_ERROR);
        }
    }

    char *payload = NULL;
    if (steg_config->action == EMBED)
    {
        size_t payload_size;
        payload = generate_payload(steg_config->in_file_path, &payload_size, p_data, should_encrypt);
        logw(DEBUG, "%s\n", "Hiding payload into meta");
        if (-1 == hide_payload_into_meta(steg_config->steg_mode, payload, bmp_metadata, payload_size))
        {
            logw(ERROR, "%s", "Error hiding payload");
            exit_clean_up(STATUS_ERROR);
        }
        if (-1 == metadata_to_file(bmp_metadata, steg_config->bmp_out_path))
        {
            logw(ERROR, "Error dumping meta in file %s", steg_config->bmp_out_path);
            exit_clean_up(STATUS_ERROR);
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
            exit_clean_up(STATUS_ERROR);
        }
        free(extracted_payload);
    }

    if (payload)
        free(payload);
    if (should_encrypt)
        clear_password_data(&p_data);
    exit_clean_up(STATUS_SUCCESS);
}

static void sigterm_handler(const int signal)
{
    logw(DEBUG, "signal %d, cleaning up and exiting\n", signal);
    exit_clean_up(EXIT_SUCCESS);
}

void exit_clean_up(int err_code)
{
    if (steg_config != NULL)
    {
        free(steg_config);
        steg_config = NULL;
    }
    if (bmp_metadata != NULL)
    {
        if (bmp_metadata->pixels != NULL)
        {
            free(bmp_metadata->pixels);
            bmp_metadata->pixels = NULL;
        }
        free(bmp_metadata);
        bmp_metadata = NULL;
    }
    if (carrier_fptr != NULL)
    {
        fclose(carrier_fptr);
    }
    exit(err_code);
}

char *generate_raw_payload(const char *in_file_path, size_t *raw_payload_size)
{

    FILE *in_fptr = fopen(steg_config->in_file_path, "rw");
    if (in_fptr == NULL)
    {
        logw(ERROR, "%s\n", "Invalid in file path.");
        exit_clean_up(STATUS_ERROR);
    }

    uint32_t file_size = get_file_size(in_fptr);
    const char *ext = get_filename_ext(in_file_path);
    printf("%s\n", ext);
    *raw_payload_size = sizeof(uint32_t) + file_size + strlen(ext) + 1;
    char *payload = calloc(*raw_payload_size, sizeof(char));
    if (payload == NULL)
    {
        logw(ERROR, "%s\n", "Couldn't allocate memory for payload.");
        exit_clean_up(STATUS_ERROR);
    }
    uint32_t be_file_size = htonl(file_size);
    memcpy(payload, &be_file_size, sizeof(uint32_t));
    copy_file_content(in_fptr, payload + sizeof(uint32_t));
    strcpy(payload + file_size + sizeof(uint32_t), ext);
    // memcpy(payload + file_size + sizeof(uint32_t), ext, strlen(ext));
    fclose(in_fptr);
    return payload;
}

char *generate_payload(const char *in_file_path, size_t *payload_size, password_data p_data, int should_encrypt)
{
    size_t raw_payload_size;
    char *raw_payload = generate_raw_payload(in_file_path, &raw_payload_size);
    if (!should_encrypt)
    {
        *payload_size = raw_payload_size;
        return raw_payload;
    }
    // Move what is below this to a separate function later.
    char *encrypted_payload = malloc(raw_payload_size + 16);
    if (encrypted_payload == NULL)
    {
        free(raw_payload);
        logw(ERROR, "%s\n", "Couldn't allocate memory for encrypted payload.");
        exit_clean_up(STATUS_ERROR);
    }
    size_t encrypted_payload_size = encrypt(&p_data, (unsigned char *)raw_payload, raw_payload_size, (unsigned char *)encrypted_payload);
    size_t bundle_size = sizeof(uint32_t) + encrypted_payload_size;
    char *bundled_payload = malloc(bundle_size + 1);
    if (bundled_payload == NULL)
    {
        free(encrypted_payload);
        free(raw_payload);
        logw(ERROR, "%s\n", "Couldn't allocate memory for bundled payload.");
        exit_clean_up(STATUS_ERROR);
    }
    uint32_t be_encrypted_payload_size = htonl(encrypted_payload_size);
    memcpy(bundled_payload, &be_encrypted_payload_size, sizeof(uint32_t));
    memcpy(bundled_payload + sizeof(uint32_t), encrypted_payload, encrypted_payload_size);
    bundled_payload[bundle_size] = '\0';
    *payload_size = bundle_size;
    free(raw_payload);
    free(encrypted_payload);
    return bundled_payload;
}

int unload_payload(unsigned char *payload, password_data *p_data, int should_encrypt, char *out_file_name)
{

    uint32_t payload_size = (payload[0] << 24) + (payload[1] << 16) + (payload[2] << 8) + payload[3];
    unsigned char *final_payload = payload;
    if (should_encrypt)
    {
        final_payload = malloc(payload_size);
        if (final_payload == NULL)
        {
            logw(ERROR, "%s\n", "Couldn't allocate memory for final payload.");
            exit_clean_up(STATUS_ERROR);
        }
        size_t dec_payload_size = decrypt(p_data, (unsigned char *)payload + 4, payload_size, (unsigned char *)final_payload);
        final_payload[dec_payload_size] = 0;
        payload_size = (final_payload[0] << 24) + (final_payload[1] << 16) + (final_payload[2] << 8) + final_payload[3];
    }

    char *found_ext = (char *)final_payload + sizeof(uint32_t) + payload_size;
    if (strcmp(found_ext, get_filename_ext(out_file_name)) != 0)
    {
        logw(ERROR, "Found extension: %s doesnt match %s extension\n", final_payload + sizeof(uint32_t) + payload_size, out_file_name);
        return -1;
    }
    // TODO: CHECK OPEN FILE ERROR AND CHECK IF EXTENSIONS DONT MATCH (SEGFAULT AFTER HERE)
    FILE *file = fopen(out_file_name, "w");
    if (file == NULL)
    {
        logw(ERROR, "Couldn't open file %s\n", out_file_name);
        return -1;
    }
    int results = fwrite(final_payload + sizeof(uint32_t), 1, payload_size, file);
    if (results < 0)
    {
        logw(DEBUG, "%s", "Error writing in out file");
    }
    fclose(file);
    if (should_encrypt)
        free(final_payload);

    return 0;
}