#pragma once

#include <string>

class PowerModel {
public:
    int estimateMilliwatts(const std::string& powerState, const std::string& ledMode) const;
};

