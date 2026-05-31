#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_http_server.h"
#include "driver/uart.h"
#include "uart_bridge.h"

#define UART_PORT UART_NUM_1
#define TXD_PIN 17
#define RXD_PIN 16
#define BUF_SIZE 1024

static const char *TAG = "WebSocket";

// Process command
static void process_command(const char *cmd)
{
    ESP_LOGI("CMD", "Received command: %s", cmd);

    uart_send(cmd);
}

// RECEIVE MSG
static esp_err_t ws_receive_message(httpd_req_t *req, char **out_msg)
{
    httpd_ws_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.type = HTTPD_WS_TYPE_TEXT;

    esp_err_t ret = httpd_ws_recv_frame(req, &frame, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get frame size");
        return ret;
    }

    if (frame.len == 0) return ESP_OK;

    frame.payload = malloc(frame.len + 1);

    if (!frame.payload) return ESP_ERR_NO_MEM;

    ret = httpd_ws_recv_frame(req, &frame, frame.len);
    if (ret != ESP_OK)
    {
        free(frame.payload);
        return ret;
    }

    frame.payload[frame.len] = '\0';

    *out_msg = (char *)frame.payload;

    return ESP_OK;
}


// SEND ACK MSG
static void send_ack(httpd_req_t *req, const char *msg)
{
    httpd_ws_frame_t frame;
    memset(&frame, 0, sizeof(frame));

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "ACK:%s", msg);

    frame.type = HTTPD_WS_TYPE_TEXT;
    frame.payload = (uint8_t *)buffer;
    frame.len = strlen(buffer);

    httpd_ws_send_frame(req, &frame);
}

// HELPER: free message safely
static void ws_free_message(char *msg)
{
    if(msg) free(msg);
}

// Websocket handler
static esp_err_t ws_handler(httpd_req_t *req)
{
    // handshake
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Websocket connection established");

        httpd_ws_frame_t frame = {
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = (uint8_t *) "REQ RECIEVED",
            .len = strlen("REQ RECIEVED"),
        };

        esp_err_t ret = httpd_ws_send_frame(req, &frame);

        if( ret != ESP_OK)
        {
            ESP_LOGE(TAG, "FAILED to send ACK packet");
        }

        return ESP_OK;
    }

    char *msg = NULL;

    esp_err_t ret = ws_receive_message(req, &msg);
    if(ret != ESP_OK)
    {
        return ret;
    }

    if (msg)
    {
        ESP_LOGI(TAG, "RX: %s", msg);

        process_command(msg);

        send_ack(req, msg);

        ws_free_message(msg);
    }

    return ESP_OK;
}

// Start Websocket server
void websocket_start(void)
{
    uart_init_bridge(); 

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
