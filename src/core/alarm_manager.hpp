#pragma once
#include <string>
#include <unordered_set>
#include "out_pin_service.hpp"
#include "esp_log.h"

class AlarmManager {
private:
    std::unordered_set<std::string> activeAlarms; 
    OutPinService& ledService;

public:
    AlarmManager(OutPinService& led) : ledService(led) {}

    void addAlarm(const std::string& id, int level) {
        if (level < 2) {
            ESP_LOGI("ALARM_MGR", "Alarm (%s) rejected. Alarm level %d, is less than 2.", id.c_str(), level);
            return;
        }

        activeAlarms.insert(id); 
        ledService.turnAllOn();     
        ESP_LOGW("ALARM_MGR", "Alarm Added! Level: %d | Active Alarm Count: %d", level, activeAlarms.size());
    }

    void removeAlarm(const std::string& id) {
        activeAlarms.erase(id);  
        ESP_LOGI("ALARM_MGR", "Alarm Removed! Active Alarm Count: %d", activeAlarms.size());
        
        if (activeAlarms.empty()) {
            ledService.turnAllOff();
            ESP_LOGI("ALARM_MGR", "All alarms silenced, LED turned off.");
        }
    }

    void handleResetAllAlarmsOfUser(const std::string& userId) {
        if (userId == RELEVANT_USER_ID) {
            activeAlarms.clear();
            ledService.turnAllOff();
            ESP_LOGI("ALARM_MGR", "All alarms reset for user: %s", userId.c_str());
        } else {
            ESP_LOGW("ALARM_MGR", "User ID mismatch. Alarms not reset. Incoming ID: %s", userId.c_str());
        }
    }

    // test to buzzers to see if they are working properly
    void runTestProtocol() {
        ledService.runTestProtocol();
    }
};