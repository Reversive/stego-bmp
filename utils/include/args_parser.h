#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "logger.h"
#include <getopt.h>

typedef enum
{
    NO_ACTION = -1,
    EMBED,
    EXTRACT
} ACTION;

typedef enum
{
    NO_STEG = -1,
    LSB1 = 0,
    LSB4 = 1,
    LSBI = 2,
} STEG_MODE;

typedef enum
{
    NO_ALGO = -1,
    AES128 = 0,
    AES192 = 1,
    AES256 = 2,
    DES = 3
} ALGO_MODE;

typedef enum
{
    NO_BLOCK = -1,
    ECB = 0,
    CFB = 1,
    OFB = 2,
    CBC = 3
} BLOCK_MODE;

typedef struct steg_configuration_t
{
    ACTION action;
    char *in_file_path;
    char *bmp_carrier_path;
    char *bmp_out_path;
    STEG_MODE steg_mode;
    ALGO_MODE algo_mode;
    BLOCK_MODE block_mode;
    char *enc_password;
} steg_configuration;

typedef steg_configuration *steg_configuration_ptr;

/*
 * Function: init_steg_config
 * ----------------------------
 *   Allocates space for the steg_configuration structure.
 *
 *   returns: steg_configuration_ptr allocated structure.
 */
steg_configuration_ptr init_steg_config();

/*
 * Function: parse_options
 * ----------------------------
 *   Parses the command line arguments and fills the steg_configuration structure.
 *   argc: number of arguments.
 *   argv: array of arguments.
 *
 *  returns: steg_configuration_ptr allocated structure.
 */
steg_configuration_ptr parse_options(int argc, char *argv[]);

#endif
