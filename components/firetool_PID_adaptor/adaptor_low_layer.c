//
// Created by taxue on 2023/1/11.
//

#include "adaptor_low_layer.h"

//接收到数据之后的回调函数
void (*received_data_cb)(AdaptorUart * adaptor_uart, char * data, int len);

//串口事件队列
QueueHandle_t uart1_queue;

/**
 * @brief 事件监听函数，可以理解为中断读取数据
 * @param pvPara
 */
void uart_event_task(void * pvPara)
{
    uart_event_t event;
    AdaptorUart * adaptorUart_rev = (AdaptorUart * )pvPara;

    while(1){
        if(xQueueReceive(uart1_queue, &event, portMAX_DELAY)){
            //UART接收到数据
            if (event.type == UART_DATA){
                //malloc大小
                uint16_t rev_size = 1024;
                char * received_data = malloc(rev_size * sizeof(char ));
                if (received_data != NULL){
                    int received_length = 0;
                    received_length = au_uart_read_bytes(adaptorUart_rev,
                                                         received_data, rev_size);
                    //调用用户自定义回调函数，进行数据处理
                    if(received_length != 0)
                    {
                        received_data_cb(adaptorUart_rev, received_data,
                                         received_length);
                    }

                    free(received_data);
                }else{
                    printf("malloc the memory failed!\n");
                    exit(0);
                }
            }
        }
    }
}

/**
 * @brief 设置UART接收到数据之后的回调函数
 * @param adaptor_uart 串口类
 * @param func 自定义的函数，参数类型应对应
 */
void au_set_received_cb(AdaptorUart * adaptor_uart,
                        void (*func)(AdaptorUart * adaptor_uart,
                                     char * data, int len))
{
    if (func != NULL) received_data_cb = func;
    else{
        printf("UART received callback function be assigned failed.\n");
        exit(0);
    }
}

/**
 * @brief 初始化串口
 * @param adaptor_uart 串口类
 * @param band_rate 波特率
 * @param rx_pin 接收引脚
 * @param tx_pin 发送引脚
 * @param uart_num 串口编号
 * @param buffer_size 接收发送缓冲区大小，此值不得小于1024
 * @note 为了防止FreeRTOS发送串口数据时阻塞任务，所以发送接收缓冲区大小并未开放单独设置
 */
void au_init_uart(AdaptorUart * adaptor_uart, int band_rate, int rx_pin,
                  int tx_pin, int uart_num, int buffer_size)
{
    //设置串口参数
    adaptor_uart->uart_config.baud_rate = band_rate;
    adaptor_uart->uart_config.data_bits = UART_DATA_8_BITS;
    adaptor_uart->uart_config.parity = UART_PARITY_DISABLE;
    adaptor_uart->uart_config.stop_bits = UART_STOP_BITS_1;
    adaptor_uart->uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    adaptor_uart->uart_config.source_clk = UART_SCLK_DEFAULT;

    //存储必要值
    adaptor_uart->uart_num = uart_num;

    //set uart param
    ESP_ERROR_CHECK(uart_param_config(uart_num, &adaptor_uart->uart_config));

    //set uart pins
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin,
                                 UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));

    //driver install
    ESP_ERROR_CHECK(uart_driver_install(uart_num, buffer_size,
                                        buffer_size, 20,
                                        &uart1_queue,0));

    //回调函数空检查
    if (received_data_cb == NULL){
        printf("The callback function of adaptor_low_layer not be assigned by self-defined function.\n");
        exit(0);
    }

    //创建串口监听
    xTaskCreate(uart_event_task, "uart_task_event", buffer_size * 3,
                adaptor_uart, 12, NULL);
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
 * @param max_len 存放接收内容的内存空间大小
 * @return 本次接收的字节个数，如果缓存已有的数据大于max_len，则只返回max_len大小的数据，
 * 反之则返回缓冲区的全部数据
 */
int au_uart_read_bytes(AdaptorUart * adaptor_uart, char * data, int max_len)
{
    int rev_data_len = 0;
    //检查接收缓冲区中是否有数据
    ESP_ERROR_CHECK(uart_get_buffered_data_len(adaptor_uart->uart_num, (size_t*)&rev_data_len));
    if(rev_data_len != 0){
        if(rev_data_len <= max_len)
            return uart_read_bytes(adaptor_uart->uart_num, data, rev_data_len,
                                   0);
        else return uart_read_bytes(adaptor_uart->uart_num, data, max_len,
                                    0);
    }
    return 0;
}


