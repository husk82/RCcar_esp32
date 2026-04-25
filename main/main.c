#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_module.h"
#include "websocket_module.h"

void app_main(void)
{
    wifi_init_softap();
    websocket_start();

    while (1)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}