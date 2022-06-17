#ifndef _LSB_H
#define _LSB_H

#include "../utils/include/args_parser.h"
#include "../utils/include/bmp_parser.h"
#include "../utils/include/logger.h"

#define GET_BIT(x, n) (((x) >> (n)) & 1)
#define SET_BIT_TO(x, n, v) ((x) = ((x) & ~(1 << (n))) | ((v) << (n)))

/*
 * Function: hide_payload_into_meta
 * ----------------------------
 *   Hides payload into bitmap_metadata struct.
 *
 *   mode: steganography mode.
 *   payload: payload to hide into metadata.
 *   metadata: bitmap metadata data.
 *   payload_size: size of payload in bytes.
 *
 *   returns: 0 on success, -1 on error.
 */
int hide_payload_into_meta(STEG_MODE mode, char *payload, bitmap_metadata_ptr metadata, size_t payload_size);

/*
 * Function: extract_payload_from_meta
 * ----------------------------
 *   Extracts payload from bitmap metadata.
 *
 *   mode: steganography mode.
 *   metadata: bitmap metadata data.
 *   was_encrypted: if the payload was encrypted.
 *
 *   returns: extracted payload from bitmap.
 */
unsigned char *extract_payload_from_meta(STEG_MODE mode, bitmap_metadata_ptr metadata, int was_encrypted);

#endif