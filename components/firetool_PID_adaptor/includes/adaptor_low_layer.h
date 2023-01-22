//
// Created by taxue on 2023/1/11.
//

#ifndef FIRETOOL_PID_DRIVER_ESP_ADAPTOR_LOW_LAYER_H
#define FIRETOOL_PID_DRIVER_ESP_ADAPTOR_LOW_LAYER_H

#include <esp_log.h>
#include "driver/uart.h"
#include "stdlib.h"

/**
 *  本库使用面向对象方式编程，理论支持多实例，但未进行多实例测试。
 *  对于本库来说，此文件位于整个库的底层，源平台为ESP32S3，因此底层使用了FreeRTOS相关，
 * 如果进行移植，请查看README.md
 *  此文件（即本库底层），使用了xTaskCreate,并且传递了创建轮询函数时的类指针，创建轮询函数
 * 时（ESP实现中断的方式）不同的类指针，调用回调函数时传递的类指针也不同，即实现面向对象多实例。
 *  此库并未进行多实例测试，最好一个项目中只使用一个串口，使用多个通道（野火助手中的通道选择）
 * 再传递不同的值。
 * **/

/** 对于本库来说，如果建立多个对象，那么其对于数据的处理方式都相同，因此中断的回调函数
 * 写在C文件中，并且此回调函数第一个参数传递当前调用此函数的类。类似this指针。**/

/**调用初始化函数之前必须先调用设置回调函数函数！！！！！！**/

//定义串口类
typedef struct{
    uart_config_t uart_config;
    int uart_num;
}AdaptorUart;

/**回调函数设置函数区，指定一个函数处理接收到的所有数据**/
void au_set_received_cb(AdaptorUart * adaptor_uart,
                        void (*func)(AdaptorUart * adaptor_uart,
                                char * data, int len));


/**功能函数区**/
//初始化串口
void au_init_uart(AdaptorUart * adaptor_uart, int band_rate,
                  int rx_pin, int tx_pin, int uart_num,
                  int buffer_size);

//发送函数
int au_uart_send_bytes(AdaptorUart * adaptor_uart, char * contents, int len);

//接收函数
int au_uart_read_bytes(AdaptorUart * adaptor_uart, char * data, int max_len);

#endif //FIRETOOL_PID_DRIVER_ESP_ADAPTOR_LOW_LAYER_H