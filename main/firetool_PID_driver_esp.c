#include <stdio.h>
#include "firetool_PID_adaptor.h"
#include <string.h>
#include "esp_log.h"

FireToolPIDAdaptor fireToolPidAdaptor;

void fta_start_callback(unsigned char channel)
{
    printf("start command!,channel: %d\n", channel);
}

void fta_stop_callback(unsigned char channel)
{
    printf("stop command!channel: %d\n", channel);
}

void fta_reset_callback(unsigned char channel)
{
    printf("reset command!channel: %d\n", channel);
}

void fta_targetValue_callback(unsigned char channel, int32_t targetValue)
{
    printf("targetValue: %ld, channel: %d\n", targetValue, channel);
}

void fta_periodValue_callback(unsigned char channel, int32_t targetValue)
{
    printf("periodValue: %ld, channel: %d\n", targetValue, channel);
}

void fta_PID_callback(unsigned char channel, float P, float I, float D)
{
    printf("channel: %d, PID:%f, %f, %f\n", channel, P, I, D);
}

void fta_test_send_cmd(void * pvParam)
{
    while(1){
        fta_send_start_cmd(&fireToolPidAdaptor, 1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_stop_cmd(&fireToolPidAdaptor, 1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_targetValue(&fireToolPidAdaptor, 1, 14960);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_periodValue(&fireToolPidAdaptor, 1, 10);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_PID(&fireToolPidAdaptor, 1, 0.05, 0, 0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        fta_send_actualValue(&fireToolPidAdaptor, 1, 45);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    /**调用初始化函数之前必须先调用设置回调函数函数！！！！！！**/
    fta_set_received_start_cb(&fireToolPidAdaptor, fta_start_callback);
    fta_set_received_stop_cb(&fireToolPidAdaptor, fta_stop_callback);
    fta_set_received_reset_cb(&fireToolPidAdaptor, fta_reset_callback);
    fta_set_received_targetValue(&fireToolPidAdaptor, fta_targetValue_callback);
    fta_set_received_periodValue(&fireToolPidAdaptor, fta_periodValue_callback);
    fta_set_received_PID(&fireToolPidAdaptor, fta_PID_callback);

    fta_init(&fireToolPidAdaptor, 115200, 18, 17, UART_NUM_1, 1024);

    xTaskCreate(fta_test_send_cmd, "fta_test_send_cmd", 1024,
                NULL, 12, NULL);
}
