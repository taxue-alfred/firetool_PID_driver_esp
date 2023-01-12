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

void app_main(void)
{
    fta_set_received_start_cb(&fireToolPidAdaptor, fta_start_callback);
    fta_set_received_stop_cb(&fireToolPidAdaptor, fta_stop_callback);
    fta_set_received_reset_cb(&fireToolPidAdaptor, fta_reset_callback);

    fta_init(&fireToolPidAdaptor, 115200, 18, 17, UART_NUM_1, 1024);
    vTaskDelay(50/portTICK_PERIOD_MS);
}
