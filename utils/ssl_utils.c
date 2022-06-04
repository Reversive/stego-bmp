#include "./include/ssl_utils.h"
#include <string.h>
#include <openssl/evp.h>

void init_password_data(password_data *password_data)
{
    const char *password = (const unsigned char *)password_data->password;
    EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), (char *)0, password, strlen(password), 1, password_data->key, password_data->iv);
}
