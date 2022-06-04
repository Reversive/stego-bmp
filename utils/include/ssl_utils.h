#ifndef _SSL_UTILS_H
#define _SSL_UTILS_H

#define KEY_SIZE 16
typedef struct password_data
{
    char *password;
    char key[KEY_SIZE];
    char iv[KEY_SIZE];
} password_data;

#endif
