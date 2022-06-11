#ifndef _SSL_UTILS_H
#define _SSL_UTILS_H
#include "./args_parser.h"
#include <string.h>
#include <openssl/evp.h>

typedef const EVP_CIPHER *(*cypher)(void);

static const cypher cyphers[4][4] = {{EVP_aes_128_ecb, EVP_aes_128_cfb, EVP_aes_128_ofb, EVP_aes_128_cbc},
                                     {EVP_aes_192_ecb, EVP_aes_192_cfb, EVP_aes_192_ofb, EVP_aes_192_cbc},
                                     {EVP_aes_256_ecb, EVP_aes_128_cfb, EVP_aes_256_ofb, EVP_aes_256_cbc},
                                     {EVP_des_ecb, EVP_des_cfb, EVP_des_ofb, EVP_des_cbc}};

typedef struct password_data
{
    char *password;
    unsigned char* key;
    unsigned char* iv;
    cypher cypher;
} password_data;

// Inits a struct with iv and key based on pre loaded password
int init_password_data(password_data *password_data, ALGO_MODE algo_mode, BLOCK_MODE block_mode);
// Frees iv and key
void clear_password_data(password_data *password_data);
int encrypt(password_data *password_data, unsigned char* plain_in, int plain_len, unsigned char * cypher_out);
int decrypt(password_data *password_data, unsigned char* cypher_in, int cypher_len, unsigned char * plain_out);


#endif
