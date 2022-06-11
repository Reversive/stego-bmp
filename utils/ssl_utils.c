#include "./include/ssl_utils.h"
#include "./include/logger.h"

int KEY_SIZE[] = {16, 24, 32, 8};
int BLOCK_SIZE[] = {16, 16, 16, 8};

int encrypt(password_data *password_data, unsigned char *plain_in, int plain_len, unsigned char *cypher_out)
{

    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())){
        logw(ERROR, "%s\n", "Couldn't create cipher context.\n");
        return -1;
    }

    if (1 != EVP_EncryptInit_ex(ctx, (password_data->cypher)(), NULL, password_data->key, password_data->iv)){
        logw(ERROR, "%s\n", "Couldn't initialize encryption.\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (1 != EVP_EncryptUpdate(ctx, cypher_out, &len, plain_in, plain_len)){
        logw(ERROR, "%s\n", "Couldn't encrypt body.\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, cypher_out + len, &len)){
        logw(ERROR, "%s\n", "Couldn't encrypt padding.\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int decrypt(password_data *password_data, unsigned char *cypher_in, int cypher_len, unsigned char *plain_out)
{

    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())){
        logw(ERROR, "%s\n", "Couldn't create cipher context.\n");
        return -1;
    }
    if (1 != EVP_DecryptInit_ex(ctx, (password_data->cypher)(), NULL, password_data->key, password_data->iv)){
        logw(ERROR, "%s\n", "Couldn't initialize decryption.\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (1 != EVP_DecryptUpdate(ctx, plain_out, &len, cypher_in, cypher_len)){
        logw(ERROR, "%s\n", "Couldn't decrypt body.\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plain_out + len, &len)){
        logw(ERROR, "%s\n", "Couldn't decrypt padding.\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int init_password_data_arrs(password_data *password_data, ALGO_MODE algo_mode)
{
    password_data->iv = malloc(BLOCK_SIZE[algo_mode]);
    if (!password_data->iv)
    {
        logw(ERROR, "%s\n", "Couldn't allocate iv.\n");
        return -1;
    }
    password_data->key = malloc(KEY_SIZE[algo_mode]);
    if (!password_data->key)
    {
        logw(ERROR, "%s\n", "Couldn't allocate key.\n");
        free(password_data->iv);
        return -1;
    }
    return 0;
}

int init_password_data(password_data *password_data, ALGO_MODE algo_mode, BLOCK_MODE block_mode)
{
    password_data->cypher = cyphers[algo_mode][block_mode];
    if (-1 == init_password_data_arrs(password_data, algo_mode)){
        logw(ERROR, "%s\n", "Couldn't init data for password.\n");
        return -1;
    }

    EVP_BytesToKey(
        (password_data->cypher)(),
        EVP_sha256(),
        (const unsigned char *)0,
        (const unsigned char *)password_data->password,
        strlen(password_data->password),
        1,
        password_data->key,
        password_data->iv);
    
    return 0;
}

void clear_password_data(password_data *password_data)
{

    if (password_data->iv)
        free(password_data->iv);
    if (password_data->key)
        free(password_data->key);


        
}