#include "./include/lsb.h"

static int LSB_SIZES[3] = {1, 4, 1};

unsigned char* extract_payload_from_meta_improved(bitmap_metadata_ptr metadata, int was_encrypted);
int hide_payload_into_meta_improved(char *payload,bitmap_metadata_ptr metadata,size_t payload_size);

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
    if (mode == LSBI){
        return hide_payload_into_meta_improved(payload,metadata,payload_size);
    }
    if (!can_store(mode, metadata, payload_size))
    {
        logw(ERROR,"BMP file not big enough to store, bits in payload: %ld // bits available: %ld",payload_size * 8,(size_t)metadata->info.header.height * metadata->info.header.width * 3 * LSB_SIZES[mode])
        return -1;
    }

    int payload_byte_idx = 0, payload_bit_idx = 0, component_idx = 0, component_bit_idx = LSB_SIZES[mode]-1;
    while ((size_t)payload_byte_idx < payload_size)
    {
        SET_BIT_TO(metadata->pixels[component_idx], component_bit_idx, GET_BIT(payload[payload_byte_idx], (7-payload_bit_idx)));
        lsb_pointer_increment(mode,&component_idx,&component_bit_idx,&payload_byte_idx,&payload_bit_idx);
    }

    return 0;
}

void get_size_from_meta(STEG_MODE mode, bitmap_metadata_ptr metadata, int size_arr[], int* start_component_idx){
    int size_idx = 0, size_bit_idx = 0, component_idx = 0, component_bit_idx = LSB_SIZES[mode]-1;

    while (size_idx < 4){
        SET_BIT_TO(size_arr[size_idx], (7-size_bit_idx), GET_BIT(metadata->pixels[component_idx], component_bit_idx));
        lsb_pointer_increment(mode,&component_idx,&component_bit_idx,&size_idx,&size_bit_idx);
    }

    *start_component_idx = component_idx;

}

unsigned char* extract_payload_from_meta(STEG_MODE mode, bitmap_metadata_ptr metadata, int was_encrypted){
    if (mode == LSBI){
        return extract_payload_from_meta_improved(metadata,was_encrypted);
    }
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
        SET_BIT_TO(extracted_payload[payload_idx], (7-payload_bit_idx), GET_BIT(metadata->pixels[component_idx], component_bit_idx));
        lsb_pointer_increment(mode,&component_idx,&component_bit_idx,&payload_idx,&payload_bit_idx);
    }

    return extracted_payload;

}


int get_mask_idx(unsigned char byte){
    char bit_1 = GET_BIT(byte, 1);
    char bit_2 = GET_BIT(byte, 2);
    return bit_1 + 2 * bit_2;
}

unsigned char get_bit_and_mask(unsigned char byte_to_insert, int bit_pos, int mask[], unsigned char mask_target){
    unsigned char bit = GET_BIT(byte_to_insert, bit_pos);
    return mask[get_mask_idx(mask_target)]? (bit+1)%2 : bit;
}

void scan_mask(bitmap_metadata_ptr metadata, char* payload, int payload_size, int mask[]){
    int byte_idx = 0, bit_idx = 0, component_idx = 4;
    while (byte_idx < payload_size){
        char bit_0 = GET_BIT(metadata->pixels[component_idx], 0);
        char payload_bit = GET_BIT(payload[byte_idx],(7-bit_idx));
        mask[get_mask_idx(metadata->pixels[component_idx])] += payload_bit == bit_0 ? 1 : -1;
        component_idx++;
        if (++bit_idx >= 8)
        {
            byte_idx++;
            bit_idx = 0;
        }
    }

    for (int i=0; i<4;i++){
        mask[i] = mask[i] > 0 ?  1 : 0;
    }

}

int hide_payload_into_meta_improved(
    char *payload,
    bitmap_metadata_ptr metadata,
    size_t payload_size)
{

    
    if (!can_store(LSBI, metadata, payload_size + 1))
    {
        logw(ERROR,"BMP file not big enough to store, bits in payload + mask: %ld // bits available: %ld",payload_size * 8 + 4,(size_t)metadata->info.header.height * metadata->info.header.width * 3 )
        return -1;
    }

    int mask[4] = {0};
    scan_mask(metadata,payload,payload_size,mask);

    for (int i = 0; i < 4; i++){
        SET_BIT_TO(metadata->pixels[i],0,mask[i]);
    }

    int payload_byte_idx = 0, payload_bit_idx = 0, component_idx = 4, component_bit_idx = 0;
    while ((size_t)payload_byte_idx < payload_size)
    {
        SET_BIT_TO(metadata->pixels[component_idx], component_bit_idx, get_bit_and_mask(payload[payload_byte_idx], (7-payload_bit_idx),mask,metadata->pixels[component_idx]));
        lsb_pointer_increment(LSBI,&component_idx,&component_bit_idx,&payload_byte_idx,&payload_bit_idx);
    }
    return 0;
}

void get_size_from_meta_masked(bitmap_metadata_ptr metadata, int size_arr[], int mask[]){
    int size_idx = 0, size_bit_idx = 0, component_idx = 4, component_bit_idx = 0;

    while (size_idx < 4){
        SET_BIT_TO(size_arr[size_idx], (7-size_bit_idx), get_bit_and_mask(metadata->pixels[component_idx], component_bit_idx,mask,metadata->pixels[component_idx]));
        lsb_pointer_increment(LSB1,&component_idx,&component_bit_idx,&size_idx,&size_bit_idx);
    }


}

unsigned char* extract_payload_from_meta_improved(bitmap_metadata_ptr metadata, int was_encrypted){
    int payload_idx = 4, payload_bit_idx = 0,component_idx = 4+32, component_bit_idx = 0;
    int mask[4] = {0};    
    for (int i = 0; i < 4; i++){
        mask[i] = GET_BIT(metadata->pixels[i],0);
        printf("mask %d: %d\n",i,GET_BIT(metadata->pixels[i],0));
    }

    int size_arr[4] = {0};

    get_size_from_meta_masked(metadata,size_arr, mask);
    int size = (size_arr[0] << 24) + (size_arr[1] << 16) + (size_arr[2] << 8) + size_arr[3];

    printf("Extracted size:%d\n",size);
    int extended_size = was_encrypted ? size: size+5;
    int total_size = sizeof(uint32_t) + extended_size;
    unsigned char * extracted_payload = malloc(total_size);

    for (int i = 0; i < 4; i++){
        extracted_payload[i] = (unsigned char)size_arr[i];
    }

    while (payload_idx < total_size){
        SET_BIT_TO(extracted_payload[payload_idx], (7-payload_bit_idx), get_bit_and_mask(metadata->pixels[component_idx], component_bit_idx,mask,metadata->pixels[component_idx]));
        lsb_pointer_increment(LSB1,&component_idx,&component_bit_idx,&payload_idx,&payload_bit_idx);
    }

    return extracted_payload;

}
