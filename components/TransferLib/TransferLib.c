//
// Created by taxue on 2023/1/12.
//

#include "TransferLib.h"

int32_t low_bit_front_transfer_int32(char * source)
{
    char data[4] = {0};
    memcpy(data, source, 4);
    int32_t * end_data = (int32_t * )data;
    return *end_data;
}