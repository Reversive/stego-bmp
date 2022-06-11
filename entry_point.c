#include "include/entry_point.h"
#include "./include/lsb.h"
#include <stdio.h>
#include <stdlib.h>

FILE *carrier_fptr, *in_fptr;
steg_configuration_ptr steg_config;
bitmap_metadata_ptr bmp_metadata;
static void sigterm_handler(const int signal);
void exit_clean_up(int err_code);
char *generate_raw_payload(const char *in_file_path, size_t *raw_payload_size);
char *generate_payload(const char *in_file_path, size_t *payload_size, password_data p_data, int should_encrypt);
int unload_payload(char* payload, password_data* p_data, int should_encrypt, char* out_file_name);

int main(
    int argc,
    char *argv[])
{
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);
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

    in_fptr = fopen(steg_config->in_file_path, "rw");
    if (in_fptr == NULL)
    {
        logw(ERROR, "%s\n", "Invalid in file path.");
        exit_clean_up(STATUS_ERROR);
    }

    password_data p_data;
    int should_encrypt = steg_config->enc_password != NULL;
    if (should_encrypt)
    {
        p_data.password = steg_config->enc_password;    
        if (-1 == init_password_data(&p_data, steg_config->algo_mode, steg_config->block_mode))
        {
            logw(ERROR, "%s\n", "Couldn't init password data.");
            exit_clean_up(STATUS_ERROR);
        }
    }

    if (steg_config->action == EMBED)
    {
        size_t payload_size;
        char *payload = generate_payload(steg_config->in_file_path, &payload_size, p_data, should_encrypt);
        // printf("Payload post-encrypt:\n");
        // for (size_t i = 0; i < payload_size; i++)
        // {
        //     printf("\\%02hhx", (unsigned char)payload[i]);
        // }
        putc('\n', stdout);

        logw(DEBUG, "%s\n", "Hiding payload into meta");
        hide_payload_into_meta(steg_config->steg_mode, payload, bmp_metadata, payload_size);
        metadata_to_file(bmp_metadata, steg_config->bmp_out_path);
        //  Move this to EXTRACT later, this is just to test the decryption.
        unload_payload(payload,&p_data,should_encrypt,"outtest.txt");
        if (should_encrypt){
            clear_password_data(&p_data);
        }
        free(payload);

    }
    else
    {
    }

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
    if (in_fptr != NULL)
    {
        fclose(in_fptr);
    }
    exit(err_code);
}

char *generate_raw_payload(const char *in_file_path, size_t *raw_payload_size)
{
    uint32_t file_size = get_file_size(in_fptr);
    const char *ext = get_filename_ext(in_file_path);
    *raw_payload_size = sizeof(uint32_t) + file_size + strlen(ext) + 1;
    char *payload = calloc(*raw_payload_size, sizeof(char));
    uint32_t be_file_size = htonl(file_size);
    memcpy(payload, &be_file_size, sizeof(uint32_t));
    copy_file_content(in_fptr, payload + sizeof(uint32_t));
    memcpy(payload + file_size + sizeof(uint32_t), ext, strlen(ext));
    // printf("Payload pre-encrypt:\n");
    // for (size_t i = 0; i < *raw_payload_size; i++)
    // {
    //     printf("\\%02hhx", (unsigned char)payload[i]);
    // }
    putc('\n', stdout);
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
    size_t encrypted_payload_size = encrypt(&p_data, (unsigned char *)raw_payload, raw_payload_size, (unsigned char *)encrypted_payload);
    size_t bundle_size = sizeof(uint32_t) + encrypted_payload_size;
    char *bundled_payload = malloc(bundle_size + 1);
    uint32_t be_encrypted_payload_size = htonl(encrypted_payload_size);
    memcpy(bundled_payload, &be_encrypted_payload_size, sizeof(uint32_t));
    memcpy(bundled_payload + sizeof(uint32_t), encrypted_payload, encrypted_payload_size);
    bundled_payload[bundle_size] = '\0';
    *payload_size = bundle_size;
    free(raw_payload);
    free(encrypted_payload);
    // BIO_dump_fp(stdout,bundled_payload,bundle_size+1);
    return bundled_payload;
}

int unload_payload(char* payload, password_data* p_data, int should_encrypt, char* out_file_name){

    uint32_t payload_size = (payload[0] << 24) + (payload[1] << 16) + (payload[2] << 8) + payload[3];
    char* final_payload = payload;
    if (should_encrypt){
        final_payload = malloc(payload_size);
        size_t dec_payload_size = decrypt(p_data, (unsigned char *)payload + 4, payload_size, (unsigned char *)final_payload);
        final_payload[dec_payload_size] = 0;
        payload_size = (final_payload[0] << 24) + (final_payload[1] << 16) + (final_payload[2] << 8) + final_payload[3];
        

    }
    //TODO: CHECK OPEN FILE ERROR AND CHECK IF EXTENSIONS DONT MATCH
    FILE* file = fopen(out_file_name,"w");
    int results = fwrite(final_payload+sizeof(uint32_t),1,payload_size,file);
    if (results < 0) {
        logw(ERROR,"%s","Error writing in out file");
    }
    fclose(file);
    if (should_encrypt) free(final_payload);

    return 0;
}