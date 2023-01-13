//
// Created by taxue on 2023/1/12.
//

#ifndef FIRETOOL_PID_DRIVER_ESP_TRANSFERLIB_H
#define FIRETOOL_PID_DRIVER_ESP_TRANSFERLIB_H

#include <string.h>

//多字节数据低字节在前转换int32
int32_t low_bit_front_transfer_int32(char * source);

//多字节数据低字节在前转换float
float low_bit_front_transfer_float(char * source);

#endif //FIRETOOL_PID_DRIVER_ESP_TRANSFERLIB_H
