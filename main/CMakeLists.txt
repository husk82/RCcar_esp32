idf_component_register(SRCS "main.c" "wifi_module.c" "websocket_module.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_wifi esp_event nvs_flash esp_http_server)