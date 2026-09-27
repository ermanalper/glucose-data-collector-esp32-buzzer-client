#pragma once
#include "interfaces/inetwork_client.hpp"
#include <functional>
#include <string>
#include "config.hpp"
// this is used to communicte with the API services, it uses the INetworkClient interface to perform GET requests and fetch data from the server
// (level 1 of the pyramid)
class BaseApiService {
protected:
    INetworkClient& networkClient;
    std::string baseUrl;

public:
    BaseApiService(INetworkClient& client, const std::string& base) 
        : networkClient(client), baseUrl(base) {}

    template <typename T>
    T get(const std::string& endpoint, std::function<T(const std::string&)> parser) {
        std::unordered_map<std::string, std::string> headers = {
            {"X-Api-Key", API_KEY} 
        };

        
        std::string rawPayload = networkClient.get(baseUrl + endpoint, headers);
        T data = {}; 
        if (!rawPayload.empty()) {
            data = parser(rawPayload);
        }
        return data;
    }

    template <typename T>
    T patch(const std::string& endpoint, std::function<T(const std::string&)> parser) {
        std::unordered_map<std::string, std::string> headers = {
            {"X-Api-Key", API_KEY} 
        };

        std::string rawPayload = networkClient.patch(baseUrl + endpoint, headers);
        T data = {}; 
        if (!rawPayload.empty() && parser) {
            data = parser(rawPayload);
        }
        return data;
    }
   
    void streamEvents(const std::string& endpoint, std::function<void(const std::string&, const std::string&)> onDataPayload) {
        
        // these headers are important for SSE (Server-Sent Events) to work properly
        // otherwise the server might buffer the data and not send it immediately
        std::unordered_map<std::string, std::string> headers = {
            {"X-Api-Key", API_KEY},
            {"Accept", "text/event-stream"},
            {"Cache-Control", "no-cache"},   
            {"Connection", "keep-alive"},     
        };

        std::string buffer = "";
        
        networkClient.stream(baseUrl + endpoint, headers, [&buffer, onDataPayload](const std::string& chunk) {
            printf("RAW CHUNK: %s\n", chunk.c_str());
            fflush(stdout); 
            buffer += chunk;
            
            while (true) {
                size_t pos = buffer.find("\n\n");
                size_t delimiter_length = 2;

                if (pos == std::string::npos) {
                    pos = buffer.find("\r\n\r\n");
                    delimiter_length = 4;
                }

                if (pos != std::string::npos) {
                    std::string eventMessage = buffer.substr(0, pos);
                    buffer.erase(0, pos + delimiter_length); 
                    
                    std::string eventType = "message"; 
                    size_t eventPos = eventMessage.find("event:");
                    if (eventPos != std::string::npos) {
                        size_t eventEnd = eventMessage.find('\n', eventPos);
                        eventType = eventMessage.substr(eventPos + 6, eventEnd - (eventPos + 6));
                        
                        size_t first = eventType.find_first_not_of(" \r");
                        if (first != std::string::npos) {
                            size_t last = eventType.find_last_not_of(" \r");
                            eventType = eventType.substr(first, (last - first + 1));
                        }
                    }

                    size_t dataPos = eventMessage.find("data:");
                    if (dataPos != std::string::npos) {
                        std::string jsonPayload = eventMessage.substr(dataPos + 5); 
                        onDataPayload(jsonPayload, eventType); 
                    }
                } else {
                    break; 
                }
            }
        });
    }
};