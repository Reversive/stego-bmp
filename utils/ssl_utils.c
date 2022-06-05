#include "./include/ssl_utils.h"

int KEY_SIZE[] = {16, 24, 32, 8};
int BLOCK_SIZE[] = {16, 16, 16, 8};

int encrypt(password_data *password_data, unsigned char *plain_in, int plain_len, unsigned char *cypher_out)
{

    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        printf("ERROR NO MANEJADO");

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_EncryptInit_ex(ctx, (password_data->cypher)(), NULL, password_data->key, password_data->iv))
        printf("ERROR NO MANEJADO");

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, cypher_out, &len, plain_in, plain_len))
        printf("ERROR NO MANEJADO");
    ciphertext_len = len;
    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, cypher_out + len, &len))
        printf("ERROR NO MANEJADO");
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(password_data *password_data, unsigned char *cypher_in, int cypher_len, unsigned char *plain_out)
{

    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        printf("ERROR NO MANEJADO");

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_DecryptInit_ex(ctx, (password_data->cypher)(), NULL, password_data->key, password_data->iv))
        printf("ERROR NO MANEJADO");

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if (1 != EVP_DecryptUpdate(ctx, plain_out, &len, cypher_in, cypher_len))
        printf("ERROR NO MANEJADO");
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, plain_out + len, &len))
        printf("ERROR NO MANEJADO");
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

void init_password_data_arrs(password_data *password_data, ALGO_MODE algo_mode)
{
    password_data->iv = malloc(BLOCK_SIZE[algo_mode]);
    password_data->key = malloc(KEY_SIZE[algo_mode]);
}

void init_password_data(password_data *password_data, ALGO_MODE algo_mode, BLOCK_MODE block_mode)
{
    password_data->cypher = cyphers[algo_mode][block_mode];
    init_password_data_arrs(password_data, algo_mode);
    EVP_BytesToKey(
        (password_data->cypher)(),
        EVP_sha256(),
        (const unsigned char *)0,
        (const unsigned char *)password_data->password,
        strlen(password_data->password),
        1,
        password_data->key,
        password_data->iv);

    // USE EXAMPLE:
    // /* Message to be encrypted */
    // unsigned char *plaintext =
    //     (unsigned char *)"The quick brown fox jumps over the lazy dog";

    // /*
    //  * Buffer for ciphertext. Ensure the buffer is long enough for the
    //  * ciphertext which may be longer than the plaintext, depending on the
    //  * algorithm and mode.
    //  */
    // unsigned char ciphertext[128];
    // unsigned char decryptedtext[128];

    // /* Encrypt the plaintext */
    // int ciphertext_len = encrypt (password_data,plaintext,ciphertext);

    // /* Do something useful with the ciphertext here */
    // printf("Plaintext is: %s\n",plaintext);
    // printf("Ciphertext is:\n");
    // BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);
    // printf("Size went from %ld to %d\n",strlen((char*)plaintext),ciphertext_len);

    //   /* Decrypt the ciphertext */
    // int decryptedtext_len = decrypt(password_data, ciphertext, ciphertext_len, decryptedtext);

    // /* Add a NULL terminator. We are expecting printable text */
    // decryptedtext[decryptedtext_len] = '\0';

    // /* Show the decrypted text */
    // printf("Text after decrypting is:\n");
    // printf("%s\n", decryptedtext);
}

void clear_password_data(password_data *password_data)
{
    free(password_data->iv);
    free(password_data->key);
}