#include "include/entry_point.h"

steg_configuration_ptr steg_config;

int main(
	int argc, 
	char *argv[]
)
{
	steg_config = parse_options(argc, argv);
	printf("%s\n", steg_config->enc_password);
	free(steg_config);
	return 0;
}