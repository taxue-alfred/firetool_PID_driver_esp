//
// Created by taxue on 2023/1/11.
//

#ifndef FIRETOOL_PID_DRIVER_ESP_ADAPTOR_LOW_LAYER_H
#define FIRETOOL_PID_DRIVER_ESP_ADAPTOR_LOW_LAYER_H

#include <esp_log.h>
#include "driver/uart.h"

//定义串口类
typedef struct{
    uart_config_t uart_config;
    int uart_num;
    int buffer_size; //缓冲区大小
}AdaptorUart;


/*回调函数设置函数区*/
void au_set_received_cb(AdaptorUart * adaptor_uart,
                        void (*func)(AdaptorUart * adaptor_uart,
                                char * data, int len));


/*功能函数区*/
//初始化串口
void au_init_uart(AdaptorUart * adaptor_uart, int band_rate,
                  int rx_pin, int tx_pin, int uart_num,
                  int buffer_size);

//发送函数
int au_uart_send_bytes(AdaptorUart * adaptor_uart, char * contents, int len);

//接收函数
int au_uart_read_bytes(AdaptorUart * adaptor_uart, char * data, int max_len);

#endif //FIRETOOL_PID_DRIVER_ESP_ADAPTOR_LOW_LAYER_H