#pragma once
#include "interfaces/inetwork_client.hpp"
#include "esp_http_client.h"
#include <string>
#include <unordered_map>
#include <functional>
class EspHttpClient : public INetworkClient {
public:
    std::string get(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers = {}) override;
    std::string patch(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers = {}) override;
    void stream(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers, std::function<void(const std::string&)> onChunk) override;
};