#include "include/args_parser.h"

steg_configuration_ptr init_steg_config()
{
	steg_configuration_ptr steg_config = malloc(sizeof(steg_configuration));
	if(steg_config == NULL)
	{
		// TO-DO: Logging
		exit(-1);
	}
	steg_config->action 			= NO_ACTION;
	steg_config->in_file_path 		= NULL;
	steg_config->bmp_carrier_path 	= NULL;
	steg_config->bmp_out_path 		= NULL;
	steg_config->steg_mode 			= NO_STEG;
	steg_config->algo_mode 			= NO_ALGO;
	steg_config->block_mode 		= NO_BLOCK;
	steg_config->enc_password 		= "secret";
	return steg_config;
}

steg_configuration_ptr parse_options(
	int argc, 
	char *argv[]
) 
{
	steg_configuration_ptr steg_config = init_steg_config();
	return steg_config;
}