#include <stdio.h>
#include "firetool_PID_adaptor.h"
#include "adaptor_low_layer.h"
#include <string.h>

AdaptorUart adaptorUart;

void app_main(void)
{
    au_init_uart(&adaptorUart, 115200, 18,
                 17, UART_NUM_1, 1024);
    char rev_content[1024];
    while(1)
    {
        char * content = "hello this is esp32s3 uart\n";
        au_uart_send_bytes(&adaptorUart, content, strlen(content) + 1);
        int len = au_uart_read_bytes(&adaptorUart, rev_content, 1024);
        rev_content[len] = '\0';
        if(len != 0)
            printf("%s\n", rev_content);
        vTaskDelay(50/portTICK_PERIOD_MS);
    }
}
