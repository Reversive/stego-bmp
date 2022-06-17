#ifndef _ENTRY_POINT_H
#define _ENTRY_POINT_H
#include "lsb.h"
#include "payload.h"
#include <signal.h>

/*
 * Function: exit_clean_up
 * ----------------------------
 *   Cleans up the memory allocated by the program and exits.
 *
 *   err_code: error code.
 *   carrier_fptr: file pointer to carrier file.
 *   steg_config: stegnography configuration.
 *   bmp_metadata: bitmap metadata.
 */
void exit_clean_up(int err_code, FILE *carrier_fptr, steg_configuration_ptr steg_config, bitmap_metadata_ptr bmp_metadata);

#endif
