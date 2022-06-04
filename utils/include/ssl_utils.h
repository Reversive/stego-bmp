#ifndef _SSL_UTILS_H
#define _SSL_UTILS_H
#include "./args_parser.h"
#include <string.h>
#include <openssl/evp.h>

#define KEY_SIZE 16

typedef const EVP_CIPHER *(*cypher)(void);

static const cypher cyphers[4][4] = {{EVP_aes_128_ecb, EVP_aes_128_cfb, EVP_aes_128_ofb, EVP_aes_128_cfb},
                                     {EVP_aes_192_ecb, EVP_aes_192_cfb, EVP_aes_192_ofb, EVP_aes_192_cfb},
                                     {EVP_aes_256_ecb, EVP_aes_128_cfb, EVP_aes_256_ofb, EVP_aes_256_cfb},
                                     {EVP_des_ecb, EVP_des_cfb, EVP_des_ofb, EVP_des_cfb}};

typedef struct password_data
{
    char *password;
    char key[KEY_SIZE];
    char iv[KEY_SIZE];
    cypher cypher;
} password_data;

void init_password_data(password_data *password_data, ALGO_MODE algo_mode, BLOCK_MODE block_mode);

#endif
