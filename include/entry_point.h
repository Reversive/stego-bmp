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
 */
void exit_clean_up(int err_code);

#endif
