#pragma once
#include <string>
#include <unordered_map>
#include <functional> 
class INetworkClient {
public:

    virtual ~INetworkClient() = default;
    virtual std::string get(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers = {}) = 0;
    virtual std::string patch(const std::string& endpoint, const std::unordered_map<std::string, std::string>& headers = {}) = 0;
    virtual void stream(const std::string& endpoint, 
                        const std::unordered_map<std::string, std::string>& headers, 
                        std::function<void(const std::string&)> onChunk) = 0;

};