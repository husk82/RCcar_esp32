#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN 2

static const char *TAG = "RC";

void led_task(void *arg)
{
    uint8_t state = 0;

    while (1)
    {
        gpio_set_level(LED_PIN, state);
        ESP_LOGI(TAG, "LED %s", state ? "ON" : "OFF");

        state = !state;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
}