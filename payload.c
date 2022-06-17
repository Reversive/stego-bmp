#include "include/payload.h"

char *generate_raw_payload(steg_configuration_ptr steg_config, size_t *raw_payload_size)
{
    FILE *in_fptr = fopen(steg_config->in_file_path, "rw");
    if (in_fptr == NULL)
    {
        logw(ERROR, "%s\n", "Invalid in file path.");
        return NULL;
    }

    uint32_t file_size = get_file_size(in_fptr);
    const char *ext = get_filename_ext(steg_config->in_file_path);
    printf("%s\n", ext);
    *raw_payload_size = sizeof(uint32_t) + file_size + strlen(ext) + 1;
    char *payload = calloc(*raw_payload_size, sizeof(char));
    if (payload == NULL)
    {
        logw(ERROR, "%s\n", "Couldn't allocate memory for payload.");
        return NULL;
    }
    uint32_t be_file_size = htonl(file_size);
    memcpy(payload, &be_file_size, sizeof(uint32_t));
    copy_file_content(in_fptr, payload + sizeof(uint32_t));
    strcpy(payload + file_size + sizeof(uint32_t), ext);
    fclose(in_fptr);
    return payload;
}

char *generate_payload(steg_configuration_ptr steg_config, size_t *payload_size, password_data p_data, int should_encrypt)
{
    size_t raw_payload_size;
    char *raw_payload = generate_raw_payload(steg_config, &raw_payload_size);
    if (raw_payload == NULL)
    {
        logw(ERROR, "%s\n", "Couldn't generate raw payload.");
        return NULL;
    }

    if (!should_encrypt)
    {
        *payload_size = raw_payload_size;
        return raw_payload;
    }

    char *encrypted_payload = malloc(raw_payload_size + 16);
    if (encrypted_payload == NULL)
    {
        free(raw_payload);
        logw(ERROR, "%s\n", "Couldn't allocate memory for encrypted payload.");
        return NULL;
    }
    size_t encrypted_payload_size = encrypt(&p_data, (unsigned char *)raw_payload, raw_payload_size, (unsigned char *)encrypted_payload);
    size_t bundle_size = sizeof(uint32_t) + encrypted_payload_size;
    char *bundled_payload = malloc(bundle_size + 1);
    if (bundled_payload == NULL)
    {
        free(encrypted_payload);
        free(raw_payload);
        logw(ERROR, "%s\n", "Couldn't allocate memory for bundled payload.");
        return NULL;
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
            return -1;
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
    FILE *file = fopen(out_file_name, "w");
    if (file == NULL)
    {
        logw(ERROR, "Couldn't open file %s\n", out_file_name);
        return -1;
    }
    int results = fwrite(final_payload + sizeof(uint32_t), 1, payload_size, file);
    int err_code = 0;
    if (results < 0)
    {
        logw(DEBUG, "%s", "Error writing in out file");
        err_code = -1;
    }
    fclose(file);
    if (should_encrypt)
    {
        free(final_payload);
    }
    return err_code;
}