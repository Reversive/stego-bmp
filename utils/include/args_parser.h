#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

enum {NO_ARG = 0, OPT_ARG, REQ_ARG} LONG_OPT_OPTS;
typedef enum {NO_ACTION = -1, EMBED, EXTRACT} ACTION;
typedef enum {NO_STEG = -1, LSB1, LSB4, LSBI} STEG_MODE;
typedef enum {NO_ALGO = -1, AES128, AES192, AES256, DES} ALGO_MODE;
typedef enum {NO_BLOCK = -1, ECB, CFB, OFB, CBC} BLOCK_MODE;

typedef struct steg_configuration_t {
	ACTION action;
	char *in_file_path;
	char *bmp_carrier_path;
	char *bmp_out_path;
	STEG_MODE steg_mode;
	ALGO_MODE algo_mode;
	BLOCK_MODE block_mode;
	char *enc_password;
} steg_configuration; 

typedef steg_configuration * steg_configuration_ptr;

extern steg_configuration_ptr steg_config;

steg_configuration_ptr init_steg_config();
steg_configuration_ptr parse_options(int argc, char *argv[]);


#endif