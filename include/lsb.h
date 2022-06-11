#ifndef _LSB_H
#define _LSB_H

#include "../utils/include/args_parser.h"
#include "../utils/include/bmp_parser.h"
#include "../utils/include/logger.h"

#define GET_BIT(x, n) ((x >> n) & 1)
#define SET_BIT_TO(x, n, v) (x = (x & ~(1 << n)) | (v << n))

int hide_payload_into_meta(STEG_MODE mode, char *payload, bitmap_metadata_ptr metadata, size_t payload_size);
char* extract_payload_from_meta(STEG_MODE mode, bitmap_metadata_ptr metadata, int was_encrypted);

#endif