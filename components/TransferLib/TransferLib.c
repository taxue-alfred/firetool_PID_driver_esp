//
// Created by taxue on 2023/1/12.
//

#include "TransferLib.h"

/**
 * @brief 多字节数据低字节在前转换int32
 * @param source 源数据
 * @return 转换值
 * @note 这里的int32是四字节的
 */
int32_t low_bit_front_transfer_int32(char * source)
{
    char data[4] = {0};
    memcpy(data, source, 4);
    int32_t * end_data = (int32_t * )data;
    return *end_data;
}

/**
 * 多字节数据低字节在前转换float
 * @param source 源数据
 * @return 转换值
 * @note 这里的float是四字节的
 */
float low_bit_front_transfer_float(char * source)
{
    char data[4] = {0};
    memcpy(data, source, 4);
    float * end_data = (float *)data;
    return  *end_data;
}