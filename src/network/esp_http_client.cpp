#include "esp_http_client.hpp"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include <functional>
//base http client implementation for ESP32 using the ESP-IDF framework
//this get function can be used with all API services that require a GET request to fetch data from the server
// (level 0 of the pyramid)
std::string EspHttpClient::get(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers) {
    std::string response_data = "";
    esp_http_client_config_t config = {};
    config.url = endpoint.c_str();
    config.method = HTTP_METHOD_GET;
    config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) return "";

    // add headers to ESP HTTP Client
    for (const auto& header : headers) {
        esp_http_client_set_header(client, header.first.c_str(), header.second.c_str());
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err == ESP_OK) {
        esp_http_client_fetch_headers(client);
        char buffer[128];
        int read_len;
        while ((read_len = esp_http_client_read(client, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[read_len] = '\0';
            response_data += buffer;
        }
    }
    esp_http_client_cleanup(client);
    return response_data;
}



void EspHttpClient::stream(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers, std::function<void(const std::string&)> onChunk) {
    esp_http_client_config_t config = {};
    config.url = endpoint.c_str();
    config.method = HTTP_METHOD_GET;
    config.crt_bundle_attach = esp_crt_bundle_attach;

    config.keep_alive_enable = true;
    config.keep_alive_idle = 2;
    config.keep_alive_interval = 2;
    config.keep_alive_count = 3;
    config.timeout_ms = 40000; 

    config.user_data = &onChunk; 
    config.event_handler = [](esp_http_client_event_t *evt) -> esp_err_t {
        if (evt->event_id == HTTP_EVENT_ON_DATA) {
            auto* cb = static_cast<std::function<void(const std::string&)>*>(evt->user_data);
            if (cb && evt->data_len > 0) {
                (*cb)(std::string((char*)evt->data, evt->data_len));
            }
        }
        return ESP_OK;
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) return;

    for (const auto& header : headers) {
        esp_http_client_set_header(client, header.first.c_str(), header.second.c_str());
    }

    esp_err_t err = esp_http_client_perform(client);
    
    esp_http_client_cleanup(client);
}

std::string EspHttpClient::patch(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers) {
    std::string response_data = "";
    esp_http_client_config_t config = {};
    config.url = endpoint.c_str();
    config.method = HTTP_METHOD_PATCH; 
    config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) return "";

    for (const auto& header : headers) {
        esp_http_client_set_header(client, header.first.c_str(), header.second.c_str());
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err == ESP_OK) {
        esp_http_client_fetch_headers(client);
        char buffer[128];
        int read_len;
        while ((read_len = esp_http_client_read(client, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[read_len] = '\0';
            response_data += buffer;
        }
    }
    esp_http_client_cleanup(client);
    return response_data;
}