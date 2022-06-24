// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/args_parser.h"

static int embed_flag, extract_flag;
static char *steg_options[3] = {"LSB1", "LSB4", "LSBI"};
static char *algo_options[4] = {"AES128", "AES192", "AES256", "DES"};
static char *block_options[4] = {"ECB", "CFB", "OFB", "CBC"};

int iequals(const char *a, const char *b)
{
    unsigned int size1 = strlen(a);
    if (strlen(b) != size1)
        return 0;
    for (unsigned int i = 0; i < size1; i++)
        if (tolower(a[i]) != tolower(b[i]))
            return 0;
    return 1;
}

void print_usage()
{
    printf("./stego-bmp [ POSIX style options ] \n"
           "POSIX style options: \n"
           "\t-embed Specifies that information will be hidden. \n"
           "\t\t-in [IN FILE PATH] Specifies the file to hide. \n"
           "\t\t-p [BITMAP FILE PATH] Specifies the bitmap carrier file. \n"
           "\t\t-out [BITMAP OUTPUT PATH] Specifies the output bmp file with the embedded file. \n"
           "\t\t-steg [LSB1 | LSB4 | LSBI] Specifies the steganography algorithm (1 bit LSB, 4 bit LSB, Enhanced LSB). \n"
           "\t-extract Specifies that information will be extracted. \n"
           "\t\t-p [BITMAP FILE PATH] Specifies the bitmap carrier file. \n"
           "\t\t-out [FILE OUTPUT PATH] Specifies the output extracted file. \n"
           "\t\t-steg [LSB1 | LSB4 | LSBI] Specifies the steganography algorithm (1 bit LSB, 4 bit LSB, Enhanced LSB). \n"
           "\tOptional arguments:\n"
           "\t\t-a [AES128 | AES192 | AES256 | DES] Specifies the hashing algorithm. \n"
           "\t\t-m [ECB | CFB | OFB | CBC] Specifies the block cypher. \n"
           "\t\t-pass [PASSWORD] Encryption password. \n");
}

static struct option long_opts[] =
    {
        {"embed", no_argument, &embed_flag, 1},
        {"extract", no_argument, &extract_flag, 1},
        {"in", required_argument, NULL, 'i'},
        {"p", required_argument, NULL, 'p'},
        {"out", required_argument, NULL, 'o'},
        {"steg", required_argument, NULL, 's'},
        {"a", required_argument, NULL, 'a'},
        {"m", required_argument, NULL, 'm'},
        {"pass", required_argument, NULL, 'j'},
        {0, 0, 0, 0}};

steg_configuration_ptr init_steg_config()
{
    steg_configuration_ptr config = malloc(sizeof(steg_configuration));
    if (config == NULL)
    {
        logw(ERROR, "%s\n", "Insufficient memory to allocate POSIX arguments.");
        exit(-1);
    }
    config->action = NO_ACTION;
    config->in_file_path = NULL;
    config->bmp_carrier_path = NULL;
    config->bmp_out_path = NULL;
    config->steg_mode = NO_STEG;
    config->algo_mode = NO_ALGO;
    config->block_mode = NO_BLOCK;
    config->enc_password = NULL;
    return config;
}

int parse_string_arg(char *input, char **options, size_t size, int default_value)
{
    for (size_t i = 0; i < size; i++)
    {
        if (iequals(input, options[i]))
        {
            return i;
        }
    }
    return default_value;
}

steg_configuration_ptr parse_options(
    int argc,
    char *argv[])
{
    if (argc == 1)
    {
        print_usage();
        exit(-1);
    }
    steg_configuration_ptr steg_config = init_steg_config();
    int option;
    while ((option = getopt_long_only(argc, argv, "eri:p:o:s:a:m:j:", long_opts, NULL)) != -1)
    {
        switch (option)
        {
        case 0:
            if (embed_flag)
            {
                steg_config->action = EMBED;
            }
            else if (extract_flag)
            {
                steg_config->action = EXTRACT;
            }
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
            break;
        default:
            fprintf(stderr, "Invalid argument option.\n");
            free(steg_config);
            exit(-1);
            break;
        }
    }

    if (embed_flag && extract_flag)
    {
        printf("Can't use extract and embed at the same time, please use either one or the other.\n");
        free(steg_config);
        exit(-1);
    }

    if (embed_flag &&
        (steg_config->in_file_path == NULL ||
         steg_config->bmp_carrier_path == NULL ||
         steg_config->bmp_out_path == NULL ||
         steg_config->steg_mode == NO_STEG))
    {
        printf("Missing arguments for embedding, make sure to provide -in <file> -p <bitmapfile> -out <bitmapfile> -steg <LSB1 | LSB4 | LSBI>\n");
        free(steg_config);
        exit(-1);
    }

    if (extract_flag &&
        (steg_config->bmp_carrier_path == NULL ||
         steg_config->bmp_out_path == NULL ||
         steg_config->steg_mode == NO_STEG))
    {
        printf("Missing arguments for extracting, make sure to provide -p <bitmapfile> -out <bitmapfile> -steg <LSB1 | LSB4 | LSBI>\n");
        free(steg_config);
        exit(-1);
    }

    if ((steg_config->algo_mode != NO_ALGO || steg_config->block_mode != NO_BLOCK) && steg_config->enc_password == NULL)
    {
        printf("Attempting to encrypt requires -pass.\n");
        free(steg_config);
        exit(-1);
    }

    if (steg_config->algo_mode == NO_ALGO)
        steg_config->algo_mode = parse_string_arg("AES128", algo_options, 4, NO_ALGO);
    if (steg_config->block_mode == NO_BLOCK)
        steg_config->block_mode = parse_string_arg("CBC", block_options, 4, NO_BLOCK);

    return steg_config;
}
