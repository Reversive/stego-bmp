#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H

typedef enum {EMBED = 0, EXTRACT} ACTION;
typedef enum {LSB1 = 0, LSB4, LSBI} STEG_MODE;
typedef enum {AES128 = 0, AES192, AES256, DES} ALGO_MODE;
typedef enum {ECB = 0, CFB, OFB, CBC} BLOCK_MODE;

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

#endif