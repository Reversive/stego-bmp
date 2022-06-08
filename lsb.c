#include "./include/lsb.h"

static int LSB_SIZES[3] = {1, 4, 0};

int can_store(STEG_MODE mode, bitmap_metadata_ptr metadata, size_t payload_size)
{
    return payload_size * 8 <= metadata->info.header.height * metadata->info.header.width * 3 * LSB_SIZES[mode];
}

int hide_payload(
    STEG_MODE mode,
    char *payload,
    bitmap_metadata_ptr metadata,
    size_t payload_size)
{
    if (!can_store(mode, metadata, payload_size))
    {
        printf("Implementa el Log flaco");
        return (-1);
    }

    size_t pixel_components = metadata->info.header.height * metadata->info.header.width * 3;

    int payload_byte_idx = 0, payload_bit_idx = 0, component_idx = 0, component_bit_idx = 0;
    while (payload_byte_idx < payload_size)
    {
        SET_BIT_TO(metadata->pixels[component_idx], component_bit_idx, GET_BIT(payload[payload_byte_idx], payload_bit_idx));

        if (++component_bit_idx >= LSB_SIZES[mode])
        {
            component_idx++;
            component_bit_idx = 0;
        }

        payload_bit_idx++;
        if (payload_bit_idx >= 8)
        {
            payload_byte_idx++;
            payload_bit_idx = 0;
        }
    }

    return 0;
}
