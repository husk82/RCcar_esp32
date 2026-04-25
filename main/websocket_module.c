#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_http_server.h"


static const char *TAG = "WebSocket";


// Websocket handler
static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Websocket connection establised");
        return ESP_OK;
    }

    char buf[128];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);

    if (ret >= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            return ESP_OK;
        }
        return ESP_FAIL;
    }

    buf[ret] = '\0';

    ESP_LOGI(TAG, "Received: %s", buf);

    // TODO: later -> forward to STM32 UART

    return ESP_OK;
}

// Start Websocket server
void websocket_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_LOGI(TAG, "Starting HTTP server...");

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t ws_uri = {
            .uri = "/ws",
            .method = HTTP_GET,
            .handler = ws_handler
        };

        httpd_register_uri_handler(server, &ws_uri);

        ESP_LOGE(TAG, "Websocket server started at /ws");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }

}
