#pragma once

#include <string>
#include <vector>
#include <cstdint>

enum VariableTypes {
    Byte,
    Byte2BigEndian,
    Byte4BigEndian,
    FloatBigEndian,
    String
};

enum ValueType {
    VT_Int,
    VT_Float,
    VT_String
};

struct CheatEntry {
    std::string description;
    VariableTypes variableType;
    uint64_t address;
    float defaultValue = 0.0f;
    float currentValue = 0.0f;

    int intValue = 0;
    float floatValue = 0.0f;
    char stringValue[256] = {0};
    int stringLength = 0;

    ValueType valueType;
    bool active = true;
    
};

extern std::vector<CheatEntry> cheatEntries;

bool loadCheatTable(const std::string& filepath);
bool ReadValueAsFloat(pid_t pid, uintptr_t address, VariableTypes type, float& outValue);
bool WriteValueFromFloat(pid_t pid, uintptr_t address, VariableTypes type, float value, const std::string& stringValue);