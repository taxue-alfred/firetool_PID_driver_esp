//
// Created by taxue on 2023/1/11.
//

#ifndef FIRETOOL_PID_DRIVER_ESP_FIRETOOL_PID_ADAPTOR_H
#define FIRETOOL_PID_DRIVER_ESP_FIRETOOL_PID_ADAPTOR_H

#include <string.h>
#include "adaptor_low_layer.h"
#include "TransferLib.h"

typedef struct {
    //继承必须放置第一位，方便后续回调函数父类种子很转换到子类指针
    AdaptorUart adaptorUart;

    /*回调函数区*/
    //启动
    void (*fta_received_start_cb)(unsigned char channel);
    //停止
    void (*fta_received_stop_cb)(unsigned char channel);
    //复位
    void (*fta_received_reset_cb)(unsigned char channel);
}FireToolPIDAdaptor;


/*回调函数设置函数区*/
//启动回调函数设置
void fta_set_received_start_cb(FireToolPIDAdaptor * fta_class,
                               void (*func)(unsigned char channel));
//停止
void fta_set_received_stop_cb(FireToolPIDAdaptor * fta_class,
                              void (*func)(unsigned char channel));
//复位
void fta_set_received_reset_cb(FireToolPIDAdaptor * fta_class,
                              void (*func)(unsigned char channel));


/*功能函数区*/
void fta_init(FireToolPIDAdaptor * fta_class, int band_rate, int rx_pin,
              int tx_pin,int uart_num, int buffer_size);

#endif //FIRETOOL_PID_DRIVER_ESP_FIRETOOL_PID_ADAPTOR_H
