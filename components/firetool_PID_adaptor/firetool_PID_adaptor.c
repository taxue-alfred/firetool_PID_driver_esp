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
        printf("malloc the memory failed!\n");
        exit(0);
    }
}

/**
 * @brief 设置接收到“启动”命令的回调函数
 * @param fta_class FireToolPIDAdaptor类
 * @param func 自定义回调函数
 */
void fta_set_received_start_cb(FireToolPIDAdaptor * fta_class,
                               void (*func)(unsigned char channel))
{
    if (func != NULL) fta_class->fta_received_start_cb = func;
}

/**
 * @brief 设置接收到“停止”命令的回调函数
 * @param fta_class FireToolPIDAdaptor类
 * @param func 自定义回调函数
 */
void fta_set_received_stop_cb(FireToolPIDAdaptor * fta_class,
                              void (*func)(unsigned char channel))
{
    if (func != NULL) fta_class->fta_received_stop_cb = func;
}

/**
 * @brief 设置接收到“复位”命令的回调函数
 * @param fta_class FireToolPIDAdaptor类
 * @param func 自定义回调函数
 */
void fta_set_received_reset_cb(FireToolPIDAdaptor * fta_class,
                               void (*func)(unsigned char channel))
{
    if (func != NULL) fta_class->fta_received_reset_cb = func;
}

/**
 * @brief 设置接收到“设定目标值”命令的回调函数
 * @param fta_class FireToolPIDAdaptor类
 * @param func 自定义回调函数
 */
void fta_set_received_targetValue(FireToolPIDAdaptor * fta_class,
                                  void(*func)(unsigned char channel, int32_t targetValue))
{
    if (func != NULL) fta_class->fta_received_targetValue = func;
}

/**
 * @brief 设置接收到“发送周期”命令的回调函数
 * @param fta_class FireToolPIDAdaptor类
 * @param func 自定义回调函数
 */
void fta_set_received_periodValue(FireToolPIDAdaptor * fta_class,
                                  void (*func)(unsigned char channel, int32_t periodValue))
{
    if (func != NULL) fta_class->fta_received_periodValue = func;
}

/**
 * @brief 设置接收到“发送PID”命令的回调函数
 * @param fta_class FireToolPIDAdaptor类
 * @param func 自定义回调函数
 */
void fta_set_received_PID(FireToolPIDAdaptor * fta_class,
                          void (*func)(unsigned char channel, float P, float I, float D))
{
    if (func != NULL) fta_class->fta_received_PID = func;
}

/**
 * @brief FireToolPIDAdaptor初始化函数
 * @param fta_class FireToolPIDAdaptor类
 * @param band_rate 波特率
 * @param rx_pin RX引脚
 * @param tx_pin TX引脚
 * @param uart_num 串口编号
 * @param buffer_size 接收发送缓冲区大小，此值不得小于1024
 * @note 为了防止FreeRTOS发送串口数据时阻塞任务，所以发送接收缓冲区大小并未开放单独设置
 * @note 调用此函数前务必先设置回调函数！！！！！
 */
void fta_init(FireToolPIDAdaptor * fta_class, int band_rate, int rx_pin, int tx_pin,
              int uart_num, int buffer_size)
{
    au_set_received_cb(&fta_class->adaptorUart, uart_received_data_cb);
    au_init_uart(&fta_class->adaptorUart, band_rate, rx_pin,
                 tx_pin, uart_num, buffer_size);

    //必要性检查
    if ((void *)fta_class != (void *)&fta_class->adaptorUart) {
        printf("The lib can`t run on this platform, please create an issue on Github!\n");
        exit(0);
    }

    assert(fta_class->fta_received_start_cb);
    assert(fta_class->fta_received_stop_cb);
    assert(fta_class->fta_received_reset_cb);
    assert(fta_class->fta_received_targetValue);
    assert(fta_class->fta_received_periodValue);
}

/**
 * @brief 发送开始指令
 * @param fta_class FireToolPIDAdaptor类
 * @param channel 通道
 * @return 已发送字节数
 */
int fta_send_start_cmd(FireToolPIDAdaptor * fta_class, unsigned char channel)
{
    int len = 0;
    char pkg_head[4] = {0x53, 0x5a, 0x48, 0x59};
    len += 4; //包头数
    len += 1; //channel变量已有，长度直接加1
    len += 4; //包长度占四个字节
    char pkg_cmd = 0x04;
    len += 1; //指令占一个字节
    char sum_auth = 0;
    len += 1; //校验占一个字节

    //先构成整个数据包，再进行校验和计算
    char * package = (char * ) malloc(len);
    char * assign_package = package;
    memcpy(assign_package, pkg_head, 4);
    assign_package += 4;
    memcpy(assign_package, &channel, 1);
    assign_package += 1;
    memcpy(assign_package, &len, 4);
    assign_package += 4;
    memcpy(assign_package, &pkg_cmd, 1);
    assign_package += 1;

    sum_auth = (char)sum_auth_for_bytes(package, len - 1);
    memcpy(assign_package, &sum_auth, 1);

    int send_number = au_uart_send_bytes(&fta_class->adaptorUart, package, len);
    if (send_number != len) assert("已有数据长度和发送数据长度不匹配！");

    free(package);
    return send_number;
}

/**
 * @brief 发送停止指令
 * @param fta_class FireToolPIDAdaptor类
 * @param channel 通道
 * @return 已发送字节数
 */
int fta_send_stop_cmd(FireToolPIDAdaptor * fta_class, unsigned char channel)
{
    int len = 0;
    char pkg_head[4] = {0x53, 0x5a, 0x48, 0x59};
    len += 4; //包头数
    len += 1; //channel变量已有，长度直接加1
    len += 4; //包长度占四个字节
    char pkg_cmd = 0x05;
    len += 1; //指令占一个字节
    char sum_auth = 0;
    len += 1; //校验占一个字节

    //先构成整个数据包，再进行校验和计算
    char * package = (char * ) malloc(len);
    char * assign_package = package;
    memcpy(assign_package, pkg_head, 4);
    assign_package += 4;
    memcpy(assign_package, &channel, 1);
    assign_package += 1;
    memcpy(assign_package, &len, 4);
    assign_package += 4;
    memcpy(assign_package, &pkg_cmd, 1);
    assign_package += 1;

    sum_auth = (char)sum_auth_for_bytes(package, len - 1);
    memcpy(assign_package, &sum_auth, 1);

    int send_number = au_uart_send_bytes(&fta_class->adaptorUart, package, len);
    if (send_number != len) assert("已有数据长度和发送数据长度不匹配！");

    free(package);
    return send_number;
}

/**
 * @brief 发送目标值
 * @param fta_class FireToolPIDAdaptor类
 * @param channel 通道
 * @param targetValue 目标值
 * @return 已发送的字节数
 */
int fta_send_targetValue(FireToolPIDAdaptor * fta_class, unsigned char channel,
                         uint32_t targetValue)
{
    int len = 0;
    char pkg_head[4] = {0x53, 0x5a, 0x48, 0x59};
    len += 4; //包头数
    len += 1; //channel变量已有，长度直接加1
    len += 4; //包长度占四个字节
    char pkg_cmd = 0x01;
    len += 1; //指令占一个字节
    len += 4; //数据占四个字节
    char sum_auth = 0;
    len += 1; //校验占一个字节

    //先构成整个数据包，再进行校验和计算
    char * package = (char * ) malloc(len);
    char * assign_package = package;
    memcpy(assign_package, pkg_head, 4);
    assign_package += 4;
    memcpy(assign_package, &channel, 1);
    assign_package += 1;
    memcpy(assign_package, &len, 4);
    assign_package += 4;
    memcpy(assign_package, &pkg_cmd, 1);
    assign_package += 1;
    memcpy(assign_package, &targetValue, 4);
    assign_package += 4;

    sum_auth = (char)sum_auth_for_bytes(package, len - 1);
    memcpy(assign_package, &sum_auth, 1);

    int send_number = au_uart_send_bytes(&fta_class->adaptorUart, package, len);
    if (send_number != len) assert("已有数据长度和发送数据长度不匹配！");

    free(package);
    return send_number;
}

/**
 * @brief 发送周期值
 * @param fta_class FireToolPIDAdaptor类
 * @param channel 通道
 * @param periodValue 周期值
 * @return 已发送的字节数
 */
int fta_send_periodValue(FireToolPIDAdaptor * fta_class, unsigned char channel,
                         uint32_t periodValue)
{
    int len = 0;
    char pkg_head[4] = {0x53, 0x5a, 0x48, 0x59};
    len += 4; //包头数
    len += 1; //channel变量已有，长度直接加1
    len += 4; //包长度占四个字节
    char pkg_cmd = 0x06;
    len += 1; //指令占一个字节
    len += 4; //数据占四个字节
    char sum_auth = 0;
    len += 1; //校验占一个字节

    //先构成整个数据包，再进行校验和计算
    char * package = (char * ) malloc(len);
    char * assign_package = package;
    memcpy(assign_package, pkg_head, 4);
    assign_package += 4;
    memcpy(assign_package, &channel, 1);
    assign_package += 1;
    memcpy(assign_package, &len, 4);
    assign_package += 4;
    memcpy(assign_package, &pkg_cmd, 1);
    assign_package += 1;
    memcpy(assign_package, &periodValue, 4);
    assign_package += 4;

    sum_auth = (char)sum_auth_for_bytes(package, len - 1);
    memcpy(assign_package, &sum_auth, 1);

    int send_number = au_uart_send_bytes(&fta_class->adaptorUart, package, len);
    if (send_number != len) assert("已有数据长度和发送数据长度不匹配！");

    free(package);
    return send_number;
}

/**
 * @brief 发送PID参数
 * @param fta_class FireToolPIDAdaptor
 * @param channel 通道
 * @param P P参数
 * @param I I参数
 * @param D D参数
 * @return 已发送的字节数
 */
int fta_send_PID(FireToolPIDAdaptor * fta_class, unsigned char channel,
                 float P, float I, float D)
{
    int len = 0;
    char pkg_head[4] = {0x53, 0x5a, 0x48, 0x59};
    len += 4; //包头数
    len += 1; //channel变量已有，长度直接加1
    len += 4; //包长度占四个字节
    char pkg_cmd = 0x03;
    len += 1; //指令占一个字节
    len += 12; //数据占12个字节
    char sum_auth = 0;
    len += 1; //校验占一个字节

    //先构成整个数据包，再进行校验和计算
    char * package = (char * ) malloc(len);
    char * assign_package = package;
    memcpy(assign_package, pkg_head, 4);
    assign_package += 4;
    memcpy(assign_package, &channel, 1);
    assign_package += 1;
    memcpy(assign_package, &len, 4);
    assign_package += 4;
    memcpy(assign_package, &pkg_cmd, 1);
    assign_package += 1;
    memcpy(assign_package, &P, 4);
    assign_package += 4;
    memcpy(assign_package, &I, 4);
    assign_package += 4;
    memcpy(assign_package, &D, 4);
    assign_package += 4;


    sum_auth = (char)sum_auth_for_bytes(package, len - 1);
    memcpy(assign_package, &sum_auth, 1);

    int send_number = au_uart_send_bytes(&fta_class->adaptorUart, package, len);
    if (send_number != len) assert("已有数据长度和发送数据长度不匹配！");

    free(package);
    return send_number;
}

/**
 * @brief 发送实际值
 * @param fta_class FireToolPIDAdaptor
 * @param channel 通道
 * @param actualValue 实际值
 * @return 已发送的字节数
 */
int fta_send_actualValue(FireToolPIDAdaptor * fta_class, unsigned char channel,
                         uint32_t actualValue)
{
    int len = 0;
    char pkg_head[4] = {0x53, 0x5a, 0x48, 0x59};
    len += 4; //包头数
    len += 1; //channel变量已有，长度直接加1
    len += 4; //包长度占四个字节
    char pkg_cmd = 0x02;
    len += 1; //指令占一个字节
    len += 4; //数据占四个字节
    char sum_auth = 0;
    len += 1; //校验占一个字节

    //先构成整个数据包，再进行校验和计算
    char * package = (char * ) malloc(len);
    char * assign_package = package;
    memcpy(assign_package, pkg_head, 4);
    assign_package += 4;
    memcpy(assign_package, &channel, 1);
    assign_package += 1;
    memcpy(assign_package, &len, 4);
    assign_package += 4;
    memcpy(assign_package, &pkg_cmd, 1);
    assign_package += 1;
    memcpy(assign_package, &actualValue, 4);
    assign_package += 4;

    sum_auth = (char)sum_auth_for_bytes(package, len - 1);
    memcpy(assign_package, &sum_auth, 1);

    int send_number = au_uart_send_bytes(&fta_class->adaptorUart, package, len);
    if (send_number != len) assert("已有数据长度和发送数据长度不匹配！");

    free(package);
    return send_number;
}

/**
 * @brief 校验和计算
 * @param source 需要计算的校验和，传入数组形式
 * @param len 传入数据的字节长度
 * @return 计算结果
 */
int sum_auth_for_bytes(const char * source, unsigned char len)
{
    int sum= 0;
    for (int i = 0; i < len; ++i) {
        sum += source[i];
    }
    return sum;
}