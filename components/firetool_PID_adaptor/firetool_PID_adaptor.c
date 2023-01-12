//
// Created by taxue on 2023/1/11.
//

#include "firetool_PID_adaptor.h"

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
                //最后一位校验，即校验和不进行编写
                printf("%d\n", command);
                switch (command) {
                    case 0x12:
                        fta_cb_p->fta_received_start_cb(channel);
                        break;
                    case 0x13:
                        fta_cb_p->fta_received_stop_cb(channel);
                        break;
                    case 0x14:
                        fta_cb_p->fta_received_reset_cb(channel);
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
    fta_class->fta_received_reset_cb == NULL)
        assert("The callback function of firetool_PID_adaptor not be assigned by self-defined function.\n");
}