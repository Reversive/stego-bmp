#include "./include/ssl_utils.h"

void init_password_data(password_data *password_data, ALGO_MODE algo_mode, BLOCK_MODE block_mode)
{
    password_data->cypher = cyphers[algo_mode][block_mode];
    EVP_BytesToKey(
        (password_data->cypher)(),
        EVP_sha256(),
        (const unsigned char *)0,
        (const unsigned char *)password_data->password,
        strlen(password_data->password),
        1,
        (unsigned char *)password_data->key,
        (unsigned char *)password_data->iv);

    printf("%s // %s", password_data->key, password_data->iv);
}
