## 1. 介绍

野火PID调试助手驱动，适用于ESP系列，基于ESP-IDF5.0开发

## 2. 答疑

ESP-IDF4.4可用吗？理论来说可用，但是未经过测试

在 `firetool_PID_adaptor` 下找到 `CMakeLists.txt` ，其中5.0版本移除了driver的默认导入，4.4默认导入 `driver` ，4.4版本理论来说可用，但是未经测试。

![image-20230115143538947](https://taxue-alfred-1253400076.cos.ap-beijing.myqcloud.com/image-20230115143538947.png)

## 3. 目录结构

项目使用两层结构编写，底层和高层，底层负责串口的初始化和数据发送，高层负责数据处理和实现功能。

**此项目使用面向对象思想编写，支持多个串口创建多个实例，但是不建议使用，建议通过野火PID调试助手的“通道”功能发送多组数据**

```shell
├─components
│  ├─firetool_PID_adaptor
│  │  │  adaptor_low_layer.c 库底层
│  │  │  CMakeLists.txt
│  │  │  firetool_PID_adaptor.c 库高层
│  │  │
│  │  └─includes
│  │          adaptor_low_layer.h
│  │          firetool_PID_adaptor.h
│  │
│  └─TransferLib
│      │  CMakeLists.txt
│      │  TransferLib.c 负责数据格式转换
│      │
│      └─includes
│              TransferLib.h
│
└─main
        CMakeLists.txt
        firetool_PID_driver_esp.c 示例应用
```

## 4. 如何移植

移植此库修改底层即可。

### 1. 修改结构体

> 修改内容在adaptor_low_layer.h下

本库源代码如下：

```c
/**调用初始化函数之前必须先调用设置回调函数函数！！！！！！**/

//定义串口类
typedef struct{
    uart_config_t uart_config;
    int uart_num;
}AdaptorUart;
```

`uart_config_t` 即为串口配置结构体，`uart_num` 即为MCU中的串口设备，比如使用第二个串口设备，在IDF中就应在`au_init_uart`中传入`UART_NUM_2`，然后赋值给`uart_num`， 移植时需要修改`AdaptorUart`中的内容。

### 2. 修改`au_init_uart()`函数

> 修改内容在adaptor_low_layer.c下

此函数主要是对上面说过的类结构体中的`uart_config_t` 以及其他成员进行赋值操作，然后安装串口（取决于MCU的开发方式）

本库部分源代码：

```c
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
}
```

### 3. 修改发送和接收函数

> 修改内容在adaptor_low_layer.c下

`au_init_uart()`将相关部分初始化好之后这里直接写发送和接收逻辑就可以。

本库源代码：

```c
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
```

### 4. 修改中断函数

> 修改内容在adaptor_low_layer.c下

中断函数的目的只有一个，发生中断之后调用回调(`received_data_cb()`)函数。

IDF中断函数的实现类似轮询方式，`STM32`等直接在对应的中断函数中调用`received_data_cb()`函数传入对应数据即可。

IDF事件监听函数：

```c
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
                    assert("malloc the memory failed!\n");
                }
            }
        }
    }
}
```

## 5. 协议参考

[电机PID控制补充篇-野火上位机串口协议介绍](https://blog.csdn.net/hbsyaaa/article/details/123966179)
