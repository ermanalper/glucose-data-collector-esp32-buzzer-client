#pragma once
#include "base_api_service.hpp"
#include "models/glucose_data.hpp"
#include <ArduinoJson.h>
#include "config.hpp"

//this uses base api service, which uses the network client, to fetch glucose data
// (level 2 of the pyramid) 
class GlucoseService : public BaseApiService {
public:
    GlucoseService(INetworkClient& client) : BaseApiService(client, BASE_API_URL) {}

    GlucoseData fetchLatestGlucose() {
        return BaseApiService::get<GlucoseData>("/api/v1/glucose/latest", [](const std::string& rawPayload) {
            GlucoseData d;
            JsonDocument doc;
            if (!deserializeJson(doc, rawPayload)) {
                d.value = doc["value"] | 0;
                d.timestamp = doc["timestamp"] | "";
                d.trend = doc["trend"] | "";
                d.source = doc["source"] | "";
                d.status = doc["status"] | "";
            }
            return d;
        });
    }
    
};