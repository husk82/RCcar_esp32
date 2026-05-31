idf_component_register(SRCS "main.c" "wifi_module.c" "websocket_module.c" "uart_bridge.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_wifi esp_event nvs_flash esp_http_server esp_driver_uart)