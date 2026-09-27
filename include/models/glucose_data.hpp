#pragma once
#include <string>

struct GlucoseData {
    int value;
    std::string timestamp;
    std::string trend;
    std::string source;
    std::string status;
};