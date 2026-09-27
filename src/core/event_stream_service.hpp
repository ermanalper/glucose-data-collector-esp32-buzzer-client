#pragma once
#include "base_api_service.hpp"
#include "models/glucose_data.hpp"
#include <ArduinoJson.h>
#include "config.hpp"
#include <functional>

// listens to the sse stream and routes the incoming events to the appropriate handlers
class EventStreamService : public BaseApiService {
public:
    EventStreamService(INetworkClient& client) : BaseApiService(client, BASE_API_URL) {}

    void startListening(
        std::function<void(GlucoseData)> onGlucoseReceived,
        std::function<void(const std::string&)> onAlarmSet,
        std::function<void(const std::string&)> onAlarmReset,
        std::function<void(const std::string&)> onResetAllAlarmsOfUser,
        std::function<void(const std::string&)> onRunTestProtocol
    ) {
        BaseApiService::streamEvents(std::string("/api/v1/events/stream?client_name=") + CLIENT_NAME, [onGlucoseReceived, onAlarmSet, onAlarmReset, onResetAllAlarmsOfUser, onRunTestProtocol](const std::string& rawJson, const std::string& eventType) {
            
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, rawJson);

            if (!error) {
                // new glucose data
                if (eventType == "new_glucose" && onGlucoseReceived) {
                    GlucoseData d;
                    d.value = doc["value"] | 0;
                    d.timestamp = doc["timestamp"] | "";
                    d.trend = doc["trend"] | "";
                    d.source = doc["source"] | "";
                    d.status = doc["status"] | "";
                    
                    onGlucoseReceived(d);
                } 
                else if (eventType == "set_alarm" && onAlarmSet) {
                    onAlarmSet(rawJson);
                }
                else if (eventType == "reset_alarm" && onAlarmReset) {
                    std::string alarmId = doc.as<std::string>();
                    
                    onAlarmReset(alarmId);
                }
                else if (eventType == "reset_all_alarms_of_user" && onResetAllAlarmsOfUser) {
                    std::string userId = doc.as<std::string>();
                    onResetAllAlarmsOfUser(userId);

                }
                else if (eventType == "run_client_test_protocol" && onRunTestProtocol) {
                    std::string  targetClient = doc.as<std::string>();
                    if (targetClient == CLIENT_NAME) {
                        onRunTestProtocol(targetClient);
                    } // else ignore, this test protocol is not for this client
                }
            } else {
                ESP_LOGE("SSE", "JSON Parse Error: %s", error.c_str());
                ESP_LOGE("SSE", "Bad Data: %s", rawJson.c_str());
            }
        });
    }

    void sendHeartbeat() {
        std::string endpoint = std::string("/api/v1/events/heartbeat?client_name=") + CLIENT_NAME;
        
        BaseApiService::patch<std::string>(endpoint, [](const std::string& raw) {
            return raw;
        });
    }
};