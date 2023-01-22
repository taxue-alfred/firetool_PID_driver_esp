//
// Created by taxue on 2023/1/11.
//

#ifndef FIRETOOL_PID_DRIVER_ESP_FIRETOOL_PID_ADAPTOR_H
#define FIRETOOL_PID_DRIVER_ESP_FIRETOOL_PID_ADAPTOR_H

#include <string.h>
#include "adaptor_low_layer.h"
#include "TransferLib.h"


/**协议参考：https://blog.csdn.net/hbsyaaa/article/details/123966179 **/

/**调用初始化函数之前必须先调用设置回调函数函数！！！！！！**/

typedef struct {
    //继承必须放置第一位，方便后续回调函数父类种子很转换到子类指针，实现多实例
    AdaptorUart adaptorUart;

    /**回调函数区**/
    //启动
    void (*fta_received_start_cb)(unsigned char channel);
    //停止
    void (*fta_received_stop_cb)(unsigned char channel);
    //复位
    void (*fta_received_reset_cb)(unsigned char channel);
    //目标值
    void (*fta_received_targetValue)(unsigned char channel, int32_t targetValue);
    //周期
    void (*fta_received_periodValue)(unsigned char channel, int32_t periodValue);
    //PID下发
    void (*fta_received_PID)(unsigned char channel, float P, float I, float D);
}FireToolPIDAdaptor;


/**回调函数设置函数区**/
//启动回调函数设置
void fta_set_received_start_cb(FireToolPIDAdaptor * fta_class,
                               void (*func)(unsigned char channel));
//停止
void fta_set_received_stop_cb(FireToolPIDAdaptor * fta_class,
                              void (*func)(unsigned char channel));
//复位
void fta_set_received_reset_cb(FireToolPIDAdaptor * fta_class,
                              void (*func)(unsigned char channel));
//目标值
void fta_set_received_targetValue(FireToolPIDAdaptor * fta_class,
                                  void(*func)(unsigned char channel, int32_t targetValue));

//周期
void fta_set_received_periodValue(FireToolPIDAdaptor * fta_class,
                                  void (*func)(unsigned char channel, int32_t periodValue));

//PID下发
void fta_set_received_PID(FireToolPIDAdaptor * fta_class,
                      void (*func)(unsigned char channel, float P, float I, float D));


/**功能函数区**/
void fta_init(FireToolPIDAdaptor * fta_class, int band_rate, int rx_pin,
              int tx_pin,int uart_num, int buffer_size);

//发送开始指令
int fta_send_start_cmd(FireToolPIDAdaptor * fta_class, unsigned char channel);

//发送结束指令
int fta_send_stop_cmd(FireToolPIDAdaptor * fta_class, unsigned char channel);

//发送目标值
int fta_send_targetValue(FireToolPIDAdaptor * fta_class, unsigned char channel,
                         uint32_t targetValue);

//发送周期值
int fta_send_periodValue(FireToolPIDAdaptor * fta_class, unsigned char channel,
                         uint32_t periodValue);

//发送PID值
int fta_send_PID(FireToolPIDAdaptor * fta_class, unsigned char channel,
                 float P, float I, float D);

//发送实际值
int fta_send_actualValue(FireToolPIDAdaptor * fta_class, unsigned char channel,
                         uint32_t actualValue);

/**辅助功能函数区**/
//校验和计算
int sum_auth_for_bytes(const char * source, unsigned char len);

#endif //FIRETOOL_PID_DRIVER_ESP_FIRETOOL_PID_ADAPTOR_H
