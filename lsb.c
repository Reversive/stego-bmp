#include "./include/lsb.h"

int hide_payload(
    STEG_MODE mode,
    char *payload,
    bitmap_metadata_ptr metadata,
    size_t payload_size)
{
    size_t pixels = metadata->info.header.height * metadata->info.header.width * 3;
    if (mode == LSB1)
    {
        printf("%ld octets disponible\n", pixels / 8);
    }
    else if (mode == LSB4)
    {
    }
    else
    {
    }

    return 0;
}