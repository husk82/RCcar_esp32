#include <string.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "uart_bridge.h"

#define UART_PORT UART_NUM_1
#define TXD_PIN 17
#define RXD_PIN 16
#define BUF_SIZE 1024

static const char *TAG = "UART_BRIDGE";

void uart_init_bridge(void)
{
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    uart_param_config(UART_PORT, &config);
    uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
    ESP_LOGI(TAG, "UART INITIALIZED");
}

void uart_send(const char *data)
{
    if (!data) return;
    uart_write_bytes(UART_PORT, data, strlen(data));
    ESP_LOGI(TAG, "TX: %s", data);
}