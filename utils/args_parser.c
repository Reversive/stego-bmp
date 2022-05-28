#include "include/args_parser.h"

static struct option long_opts[] =
{
	{"embed", NO_ARG, 0, 'e'},
	{"extract", NO_ARG, 0, 'r'},
	{"in", REQ_ARG, 0, 'i'},
	{"p", REQ_ARG, 0, 'p'},
	{"out", REQ_ARG, 0, 'o'},
	{"steg", REQ_ARG, 0, 's'},
	{"a", REQ_ARG, 0, 'a'},
	{"m", REQ_ARG, 0, 'm'},
	{"pass", REQ_ARG, 0, 'j'}
};


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
	int option;
	while((option = getopt_long(argc, argv, "eri:p:o:s:a:m:j:", long_opts, NULL)) != -1)
	{
		switch (option)
		{
		case 'e':
			printf("Mode is embed\n");
			break;
		case 'r':
			printf("Mode is extract\n");
			break;
		default:
			break;
		}
	}
	return steg_config;
}