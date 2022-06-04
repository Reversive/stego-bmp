#include "./include/ssl_utils.h"
#include <string.h>
#include <openssl/evp.h>

void init_password_data(password_data *password_data)
{
    EVP_BytesToKey(
        EVP_aes_128_cbc(),
        EVP_md5(),
        (const unsigned char *)0,
        (const unsigned char *)password_data->password,
        strlen(password_data->password),
        1,
        (unsigned char *)password_data->key,
        (unsigned char *)password_data->iv);
}
