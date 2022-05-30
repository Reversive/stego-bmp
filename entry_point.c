#include "include/entry_point.h"

steg_configuration_ptr steg_config;

int main(
	int argc, 
	char *argv[]
)
{
	steg_config = parse_options(argc, argv);
	free(steg_config);
	return 0;
}