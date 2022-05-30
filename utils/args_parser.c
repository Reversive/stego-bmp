#include "include/args_parser.h"


static int embed_flag, extract_flag;
static char *steg_options[3] = {"LSB1", "LSB4", "LSBI"};
static char *algo_options[4] = {"AES128", "AES192", "AES256", "DES"};
static char *block_options[4] = {"ECB", "CFB", "OFB", "CBC"};

int iequals(const char* a, const char* b){
    unsigned int size1 = strlen(a);
    if (strlen(b) != size1)
        return 0;
    for (unsigned int i = 0; i < size1; i++)
        if (tolower(a[i]) != tolower(b[i]))
            return 0;
    return 1;
}

static struct option long_opts[] =
{
	{"embed", no_argument, &embed_flag, 0},
	{"extract", no_argument, &extract_flag, 0},
	{"in", required_argument, NULL, 'i'},
	{"p", required_argument, NULL, 'p'},
	{"out", required_argument, NULL, 'o'},
	{"steg", required_argument, NULL, 's'},
	{"a", required_argument, NULL, 'a'},
	{"m", required_argument, NULL, 'm'},
	{"pass", required_argument, NULL, 'j'},
	{0, 0, 0, 0}
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
	steg_config->enc_password 		= NULL;
	return steg_config;
}

int parse_string_arg(char *input, char **options, size_t size, int default_value)
{
	for(int i = 0; i < size; i++)
	{
		if(iequals(input, options[i]))
		{
			return i;
		}
	}
	return default_value;
}

steg_configuration_ptr parse_options(
	int argc, 
	char *argv[]
) 
{
	steg_configuration_ptr steg_config = init_steg_config();
	int option;
	while((option = getopt_long_only(argc, argv, "eri:p:o:s:a:m:j:", long_opts, NULL)) != -1)
	{
		switch (option)
		{
		case 0:
			// Flag cases.
			break;
		case 'i':
			steg_config->in_file_path = optarg;
			break;
		case 'p':
			steg_config->bmp_carrier_path = optarg;
			break;
		case 'o':
			steg_config->bmp_out_path = optarg;
			break;
		case 's':
			steg_config->steg_mode = parse_string_arg(optarg, steg_options, 3, NO_STEG);
			break;
		case 'a':
			steg_config->algo_mode = parse_string_arg(optarg, algo_options, 4, NO_ALGO);
			break;
		case 'm':
			steg_config->block_mode = parse_string_arg(optarg, block_options, 4, NO_BLOCK);
			break;
		case 'j':
			steg_config->enc_password = optarg;
			printf ("option -pass (-j) with value `%s'\n", optarg);
			break;
		default:
			fprintf(stderr, "Invalid argument options\n");
			exit(-1);
			break;
		}
	}

	if(embed_flag && extract_flag)
	{
		printf("Can't use extract and embed at the same time, please use one or the other.\n");
		free(steg_config);
		exit(-1);
	}

	return steg_config;
}