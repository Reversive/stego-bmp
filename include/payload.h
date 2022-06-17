#ifndef _PAYLOAD_H
#define _PAYLOAD_H
#include "../utils/include/ssl_utils.h"
#include "../utils/include/file.h"
#include <inttypes.h>
#include <arpa/inet.h>
/*
 * Function: generate_raw_payload
 * ----------------------------
 *   Generates payload from file without encryption
 *
 *   steg_config: stegnography configuration.
 *   raw_payload_size: holds the value of the raw generated payload size.
 *
 *   returns: pointer to the generated raw payload
 */
char *generate_raw_payload(steg_configuration_ptr steg_config, size_t *raw_payload_size);

/*
 * Function: generate_payload
 * ----------------------------
 *   Generates payload from file with encryption (if necessary)
 *
 *   steg_config: stegnography configuration
 *   payload_size: holds the value of the generated payload size
 *   p_data: structure holding the encryption data
 *   should_encrypt: if true, the payload will be encrypted, if not, it will call generate_raw_payload
 *   returns: pointer to the generated payload
 */
char *generate_payload(steg_configuration_ptr steg_config, size_t *payload_size, password_data p_data, int should_encrypt);

/*
 * Function: generate_payload
 * ----------------------------
 *   Dumps pre-generated payload into file
 *
 *   in_file_path: path of the file to be put into the payload
 *   payload_size: holds the value of the generated payload size
 *
 *   returns: pointer to the generated payload
 */
int unload_payload(unsigned char *payload, password_data *p_data, int should_encrypt, char *out_file_name);
#endif