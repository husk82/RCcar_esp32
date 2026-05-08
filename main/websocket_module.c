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
        ESP_LOGI(TAG, "Websocket connection established");

        httpd_ws_frame_t ws_pkt = {
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = (uint8_t *) "REQ RECIEVED",
            .len = strlen("REQ RECIEVED"),
        };

        esp_err_t ret = httpd_ws_send_frame(req, &ws_pkt);

        if( ret != ESP_OK)
        {
            ESP_LOGE(TAG, "FAILED to send ACK packet");
        }

        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(ws_pkt));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    // Get frame length
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get frame len: %d", ret);
        return ret;
    }

    if (ws_pkt.len)
    {
        // Allocate buffer
        ws_pkt.payload = malloc(ws_pkt.len + 1);
        if (!ws_pkt.payload)
        {
            ESP_LOGE(TAG, "Malloc failed");
            return ESP_ERR_NO_MEM;
        }
        
        // Receive data
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            free(ws_pkt.payload);
            ESP_LOGE(TAG, "Failed to read frame: %d", ret);
            return ret;
        }

        ((char *)ws_pkt.payload)[ws_pkt.len] = '\0';

        ESP_LOGE(TAG, "Received: %s", (char *)ws_pkt.payload);

        // Send ACK back
        httpd_ws_frame_t ws_res;
        memset(&ws_res, 0, sizeof(ws_res));

        ws_res.type = HTTPD_WS_TYPE_TEXT;

        char ack_msg[64];
        snprintf(ack_msg, sizeof(ack_msg), "ACK:%s", (char *)ws_pkt.payload);

        ws_res.payload = (uint8_t *)ack_msg;
        ws_res.len = strlen(ack_msg);

        httpd_ws_send_frame(req, &ws_res);

        //TODO - SEND TO STM32 UART
        
        free(ws_pkt.payload);
    }
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
            .handler = ws_handler,
            .is_websocket = true
        };

        httpd_register_uri_handler(server, &ws_uri);

        ESP_LOGE(TAG, "Websocket server started at /ws");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }

}
