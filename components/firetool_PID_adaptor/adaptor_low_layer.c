//
// Created by taxue on 2023/1/11.
//

#include "adaptor_low_layer.h"

/**
 * @brief 初始化串口
 * @param adaptor_uart 串口类
 * @param band_rate 波特率
 * @param rx_pin 接收引脚
 * @param tx_pin 发送引脚
 * @param uart_num 串口编号
 * @param buffer_size 接收发送缓冲区大小
 * @note 为了防止FreeRTOS发送串口数据时阻塞任务，所以发送缓冲区大小并未开放单独设置
 */
void au_init_uart(AdaptorUart * adaptor_uart, int band_rate, int rx_pin,
                  int tx_pin, int uart_num, int buffer_size)
{
    adaptor_uart->uart_config.baud_rate = band_rate;
    adaptor_uart->uart_config.data_bits = UART_DATA_8_BITS;
    adaptor_uart->uart_config.parity = UART_PARITY_DISABLE;
    adaptor_uart->uart_config.stop_bits = UART_STOP_BITS_1;
    adaptor_uart->uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    adaptor_uart->uart_config.source_clk = UART_SCLK_DEFAULT;

    adaptor_uart->uart_num = uart_num;

    //set uart param
    ESP_ERROR_CHECK(uart_param_config(uart_num, &adaptor_uart->uart_config));

    //set uart pins
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin,
                                 UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));

    //driver install
    ESP_ERROR_CHECK(uart_driver_install(uart_num, buffer_size,
                                        buffer_size, 0,NULL,
                                        0));

}

/**
 * @brief 发送字节函数
 * @param adaptor_uart 串口类
 * @param contents 发送内容
 * @param len 发送的字节个数
 * @return 发送的字节个数
 */
int au_uart_send_bytes(AdaptorUart * adaptor_uart, char * contents, int len)
{
    return uart_write_bytes(adaptor_uart->uart_num, contents, len);
}

/**
 * @brief 接收字节函数
 * @param adaptor_uart 串口类
 * @param data 存放接收内容的内存空间
 * @param len 接收个数
 * @return 本次接收的字节个数
 */
int au_uart_read_bytes(AdaptorUart * adaptor_uart, char * data, int len)
{
    int rev_data_len = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(adaptor_uart->uart_num, (size_t*)&rev_data_len));
    if(rev_data_len != 0){
        return uart_read_bytes(adaptor_uart->uart_num, data, rev_data_len,
                               0);
    }
    return 0;
}


