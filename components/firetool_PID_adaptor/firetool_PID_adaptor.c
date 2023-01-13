//
// Created by taxue on 2023/1/11.
//

#include "firetool_PID_adaptor.h"

/**
 * @brief 底层的回调函数，负责处理数据
 * @param adaptor_uart 底层串口类
 * @param data 底层返回的数据
 * @param len 底层读取到的数据长度
 */
void uart_received_data_cb(AdaptorUart * adaptor_uart, char * data, int len)
{
    //从父类指针转换到子类指针
    FireToolPIDAdaptor * fta_cb_p = (FireToolPIDAdaptor *)adaptor_uart;

    char * data_rev = malloc(len * sizeof(char ));
    if (data_rev != NULL){
        memcpy(data_rev, data, len);
        ESP_LOGI("Test", "Read %d bytes", len);
        ESP_LOG_BUFFER_HEXDUMP("Read DATA", data, len, ESP_LOG_INFO);

        //数据处理
        char head[4] = {0x53, 0x5a, 0x48, 0x59};
        for (int i = 0; i < len; i++) {
            if (data_rev[i] == head[0] && data_rev[i+1] == head[1] && data_rev[i+2] == head[2]
            && data_rev[i+3] == head[3]){
                printf("Get the package from FireTool!\n");
                i = i + 4;
                unsigned char channel = data_rev[i];
                i = i + 1;
                char package_length_source[4] = {0};
                memcpy(package_length_source, &data_rev[i], 4);
                int32_t package_length = low_bit_front_transfer_int32(package_length_source);
                i = i + 4;
                char command = data_rev[i];
                //最后一位校验位，即校验和不进行编写
                switch (command) {
                    case 0x12: //启动
                        fta_cb_p->fta_received_start_cb(channel);
                        break;
                    case 0x13: //停止
                        fta_cb_p->fta_received_stop_cb(channel);
                        break;
                    case 0x14: //复位
                        fta_cb_p->fta_received_reset_cb(channel);
                        break;
                    case 0x11: //目标值
                        i = i + 1;
                        char data_command_source[4] = {0};
                        memcpy(data_command_source, &data_rev[i], 4);
                        int32_t data_command = low_bit_front_transfer_int32(data_command_source);
                        fta_cb_p->fta_received_targetValue(channel, data_command);
                        break;
                    case 0x15: //周期
                        i = i + 1;
                        char data_command_source_period[4] = {0};
                        memcpy(data_command_source_period, &data_rev[i], 4);
                        int32_t data_command_period = low_bit_front_transfer_int32(data_command_source_period);
                        fta_cb_p->fta_received_periodValue(channel, data_command_period);
                        break;
                    case 0x10: //PID值
                        i = i + 1;
                        char p_data_source[4] = {0};
                        char i_data_source[4] = {0};
                        char d_data_source[4] = {0};
                        memcpy(p_data_source, &data_rev[i], 4);
                        float p_data = low_bit_front_transfer_float(p_data_source);
                        i = i + 4;
                        memcpy(i_data_source, &data_rev[i], 4);
                        float i_data = low_bit_front_transfer_float(i_data_source);
                        i = i + 4;
                        memcpy(d_data_source, &data_rev[i], 4);
                        float d_data = low_bit_front_transfer_float(d_data_source);
                        fta_cb_p->fta_received_PID(channel, p_data, i_data, d_data);
                        break;
                    default:
                        printf("Incompatible instruction! Please create an issue on Github!\n");
                        break;
                }
            }
        }

        free(data_rev);
    }else{
        assert("malloc the memory failed!\n");
    }
}

void fta_set_received_start_cb(FireToolPIDAdaptor * fta_class,
                               void (*func)(unsigned char channel))
{
    if (func != NULL) fta_class->fta_received_start_cb = func;
}

//停止
void fta_set_received_stop_cb(FireToolPIDAdaptor * fta_class,
                              void (*func)(unsigned char channel))
{
    if (func != NULL) fta_class->fta_received_stop_cb = func;
}

//复位
void fta_set_received_reset_cb(FireToolPIDAdaptor * fta_class,
                               void (*func)(unsigned char channel))
{
    if (func != NULL) fta_class->fta_received_reset_cb = func;
}

//目标值
void fta_set_received_targetValue(FireToolPIDAdaptor * fta_class,
                                  void(*func)(unsigned char channel, int32_t targetValue))
{
    if (func != NULL) fta_class->fta_received_targetValue = func;
}

//周期
void fta_set_received_periodValue(FireToolPIDAdaptor * fta_class,
                                  void (*func)(unsigned char channel, int32_t periodValue))
{
    if (func != NULL) fta_class->fta_received_periodValue = func;
}

void fta_set_received_PID(FireToolPIDAdaptor * fta_class,
                          void (*func)(unsigned char channel, float P, float I, float D))
{
    if (func != NULL) fta_class->fta_received_PID = func;
}

void fta_init(FireToolPIDAdaptor * fta_class, int band_rate, int rx_pin, int tx_pin,
              int uart_num, int buffer_size)
{
    au_set_received_cb(&fta_class->adaptorUart, uart_received_data_cb);
    au_init_uart(&fta_class->adaptorUart, band_rate, rx_pin,
                 tx_pin, uart_num, buffer_size);

    //必要性检查
    if (fta_class != &fta_class->adaptorUart)
        assert("The lib can`t run on this platform, please create an issue on Github!\n");

    if (fta_class->fta_received_start_cb == NULL || fta_class->fta_received_stop_cb == NULL ||
    fta_class->fta_received_reset_cb == NULL || fta_class->fta_received_targetValue == NULL ||
    fta_class->fta_received_periodValue == NULL)
        assert("The callback function of firetool_PID_adaptor not be assigned by self-defined function.\n");
}