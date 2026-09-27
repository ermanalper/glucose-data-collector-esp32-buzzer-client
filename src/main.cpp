#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "network/esp_http_client.hpp"
#include "core/glucose_service.hpp"
#include "core/event_stream_service.hpp"
#include "core/alarm_manager.hpp"
#define ALARM_PIN1 GPIO_NUM_16
#define ALARM_PIN2 GPIO_NUM_25
static const char *TAG = "APP_MAIN";
struct TaskArgs {
    EventStreamService* streamService;
    AlarmManager* alarmManager;
};
void heartbeat_task(void *pvParameters) {
    EventStreamService* streamService = (EventStreamService*)pvParameters;
    
    while(true) {
        streamService->sendHeartbeat();
        ESP_LOGI("HEARTBEAT", "Ping: %s", CLIENT_NAME);
        
 
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}
void connect_wifi() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = {};
    snprintf((char*)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", WIFI_SSID);
    snprintf((char*)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", WIFI_PASS);

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    ESP_LOGI(TAG, "WiFi Baglaniliyor...");
    vTaskDelay(pdMS_TO_TICKS(5000)); 
}
void glucose_task(void *pvParameters) {
    GlucoseService* service = (GlucoseService*)pvParameters;
    while(true) {
        GlucoseData data = service->fetchLatestGlucose();
        ESP_LOGI("GLUCOSE", "Value: %d", data.value);
        vTaskDelay(pdMS_TO_TICKS(300000));
    }
}

void sse_listener_task(void *pvParameters) {
    TaskArgs* args = (TaskArgs*)pvParameters;
    EventStreamService* streamService = args->streamService;
    AlarmManager* alarmManager = args->alarmManager;
    
    while(true) {
        ESP_LOGI("SSE", "Stream baglantisi kuruluyor...");
        
        streamService->startListening(
            // 1. Glikoz
            [](GlucoseData data) {
                ESP_LOGI("GLUCOSE", "New GLUCOSE: %d", data.value);
                fflush(stdout);
            },
            
            // 2. Alarm Set
            [alarmManager](const std::string& alarmJson) {
                JsonDocument doc;
                if (!deserializeJson(doc, alarmJson)) {
                    std::string id = doc["id"] | "unknown";
                    int level = doc["level"] | 2; // if level is not present, default to 2 so it sets a alarm.
                    //                           always expect for the worst
                    alarmManager->addAlarm(id, level);
                }
                fflush(stdout);
            },
            
            // 3. Alarm Reset
            [alarmManager](const std::string& alarmId) {
                alarmManager->removeAlarm(alarmId);
                fflush(stdout);
            },
            [alarmManager](const std::string& userId) {
                // alarm manager checks if the userId matches the relevant user id and resets all alarms if it does.
                alarmManager->handleResetAllAlarmsOfUser(userId); 
                fflush(stdout);
            },
            [alarmManager](const std::string& targetClient) {
                // test protocol is for this client, run the test protocol
                alarmManager->runTestProtocol();
                fflush(stdout);
            }
        );
        
        ESP_LOGE("SSE", "Stream lost! Reconnecting...");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
extern "C" void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    connect_wifi();

    // Dependency Injection
    static OutPinService ledService(ALARM_PIN1, ALARM_PIN2);
    static AlarmManager alarmManager(ledService);
    
    static EspHttpClient httpClient; 
    static EventStreamService eventStreamService(httpClient); 

    static TaskArgs args = { &eventStreamService, &alarmManager };
    xTaskCreate(&sse_listener_task, "sse_task", 10240, &args, 5, NULL);
    xTaskCreate(&heartbeat_task, "heartbeat_task", 4096, &eventStreamService, 3, NULL);
}