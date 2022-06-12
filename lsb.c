#include "./include/lsb.h"

// SHOULD BE THIS
//static int LSB_SIZES[3] = {1, 4, 0};
static int LSB_SIZES[3] = {8, 8, 0};

int can_store(STEG_MODE mode, bitmap_metadata_ptr metadata, size_t payload_size)
{
    return payload_size * 8 <= (size_t)metadata->info.header.height * metadata->info.header.width * 3 * LSB_SIZES[mode];
}

// Represents movin(g a bit pointer between a straigth char array and a bmp file at the same time
void lsb_pointer_increment(STEG_MODE mode, int* bmp_byte_idx, int* bmp_bit_idx, int* arr_byte_idx, int* arr_bit_idx){
        
        if (--(*bmp_bit_idx) < 0)
        {
            (*bmp_byte_idx)++;
            *bmp_bit_idx = LSB_SIZES[mode]-1;
        }

        if (++(*arr_bit_idx) >= 8)
        {
            (*arr_byte_idx)++;
            *arr_bit_idx = 0;
        }
}

int hide_payload_into_meta(
    STEG_MODE mode,
    char *payload,
    bitmap_metadata_ptr metadata,
    size_t payload_size)
{
    if (!can_store(mode, metadata, payload_size))
    {
        logw(ERROR,"BMP file not big enough to store, bits in payload: %ld // bits available: %ld",payload_size * 8,(size_t)metadata->info.header.height * metadata->info.header.width * 3 * LSB_SIZES[mode])
        return -1;
    }

    int payload_byte_idx = 0, payload_bit_idx = 0, component_idx = 0, component_bit_idx = LSB_SIZES[mode]-1;
    while ((size_t)payload_byte_idx < payload_size)
    {
        // SHOULD BE THIS
        //SET_BIT_TO(metadata->pixels[component_idx], component_bit_idx, GET_BIT(payload[payload_byte_idx], payload_bit_idx));
        SET_BIT_TO(metadata->pixels[component_idx], component_bit_idx, 0));
        lsb_pointer_increment(mode,&component_idx,&component_bit_idx,&payload_byte_idx,&payload_bit_idx);
    }

    return 0;
}

void get_size_from_meta(STEG_MODE mode, bitmap_metadata_ptr metadata, int size_arr[], int* start_component_idx){
    int size_idx = 0, size_bit_idx = 0, component_idx = 0, component_bit_idx = LSB_SIZES[mode]-1;

    while (size_idx < 4){
        SET_BIT_TO(size_arr[size_idx], size_bit_idx, GET_BIT(metadata->pixels[component_idx], component_bit_idx));
        lsb_pointer_increment(mode,&component_idx,&component_bit_idx,&size_idx,&size_bit_idx);
    }

    *start_component_idx = component_idx;

}

unsigned char* extract_payload_from_meta(STEG_MODE mode, bitmap_metadata_ptr metadata, int was_encrypted){
    int payload_idx = 4, payload_bit_idx = 0,component_idx = 0,component_bit_idx = LSB_SIZES[mode]-1;
    int size_arr[4] = {0};
    get_size_from_meta(mode,metadata,size_arr, &component_idx);
    int size = (size_arr[0] << 24) + (size_arr[1] << 16) + (size_arr[2] << 8) + size_arr[3];

    printf("Extracted size:%d\n",size);
    int extended_size = was_encrypted ? size: size+5;
    int total_size = sizeof(uint32_t) + extended_size;
    unsigned char * extracted_payload = malloc(total_size);
    for (int i = 0; i < 4; i++){
        extracted_payload[i] = (unsigned char)size_arr[i];
    }

    while (payload_idx < total_size){
        SET_BIT_TO(extracted_payload[payload_idx], payload_bit_idx, GET_BIT(metadata->pixels[component_idx], component_bit_idx));
        lsb_pointer_increment(mode,&component_idx,&component_bit_idx,&payload_idx,&payload_bit_idx);
    }

    return extracted_payload;

}