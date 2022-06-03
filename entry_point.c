#include "include/entry_point.h"

FILE *carrier_fptr;
steg_configuration_ptr steg_config;
bitmap_metadata_ptr bmp_metadata;

void exit_clean_up(int err_code)
{
    if (steg_config != NULL)
    {
        free(steg_config);
        steg_config = NULL;
    }
    if (bmp_metadata != NULL)
    {
        free(bmp_metadata);
        bmp_metadata = NULL;
    }
    if (carrier_fptr != NULL)
    {
        fclose(carrier_fptr);
    }
    exit(err_code);
}

int main(
    int argc,
    char *argv[])
{
    steg_config = parse_options(argc, argv);
    carrier_fptr = fopen(steg_config->bmp_carrier_path, "rw");
    if (carrier_fptr == NULL)
    {
        log(ERROR, "%s\n", "Invalid carrier file path.");
        exit_clean_up(STATUS_ERROR);
    }

    bmp_metadata = bitmap_read_metadata(carrier_fptr);
    if (bmp_metadata == NULL)
    {
        exit_clean_up(STATUS_ERROR);
    }

    bitmap_header header = bmp_metadata->header;
    bitmap_info info = bmp_metadata->info;

    printf("BitmapHeader:\n");
    printf("  Header:     %c%c (0x%2x)\n", header.magic.header[0], header.magic.header[1], header.magic.id);
    printf("  Size:       0x%8x\n", header.size);
    printf("  Reserved 0: 0x%4x\n", header.reserved[0]);
    printf("  Reserved 1: 0x%4x\n", header.reserved[1]);
    printf("  Offset:     0x%8x\n", header.offset);
    printf("  BitmapInfoHeader:\n");
    printf("    Size:            0x%x\n", info.header.size);
    printf("    Width:           0x%x\n", info.header.width);
    printf("    Height:          0x%x\n", info.header.height);
    printf("    Planes:          0x%2x\n", info.header.planes);
    printf("    BitCount:        0x%2x\n", info.header.bit_count);
    printf("    Compression:     0x%2x\n", info.header.compression);
    printf("    SizeImage:       0x%x\n", info.header.size_image);
    printf("    XPelsPerMeter:   0x%x\n", info.header.x_pels_per_meter);
    printf("    YPelsPerMeter:   0x%x\n", info.header.y_pels_per_meter);
    printf("    UsedColors:      0x%2x\n", info.header.used_colors);
    printf("    ImportantColors: 0x%2x\n", info.header.important_colors);
    exit_clean_up(STATUS_SUCCESS);
    return 0;
}
