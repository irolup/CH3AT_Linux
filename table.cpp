#include "table.h"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <sstream>
#include <iostream>
#include "patch_helpers.h"

std::vector<CheatEntry> cheatEntries;

bool loadCheatTable(const std::string& filepath) {
    cheatEntries.clear();

    try {
        std::ifstream file(filepath);
        if (!file) {
            std::cerr << "Failed to open file: " << filepath << "\n";
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        std::vector<char> xmlData(content.begin(), content.end());
        xmlData.push_back('\0');

        rapidxml::xml_document<> doc;
        doc.parse<0>(&xmlData[0]);

        auto* cheatEntriesNode = doc.first_node("CheatTable");
        if (!cheatEntriesNode || !(cheatEntriesNode = cheatEntriesNode->first_node("CheatEntries"))) {
            std::cerr << "Invalid .CT structure (missing CheatTable or CheatEntries)\n";
            return false;
        }

        for (auto* entryNode = cheatEntriesNode->first_node("CheatEntry"); entryNode; entryNode = entryNode->next_sibling()) {
            CheatEntry entry;

            auto* descNode = entryNode->first_node("Description");
            if (!descNode) continue;
            entry.description = descNode->value();

            auto* typeNode = entryNode->first_node("VariableType");
            if (!typeNode) continue;
            std::string type = typeNode->value();

            std::string customType;
            if (auto* customTypeNode = entryNode->first_node("CustomType"))
                customType = customTypeNode->value();

            if (type == "Byte") entry.variableType = Byte;
            else if (type == "Custom" && customType == "2 Byte Big Endian") entry.variableType = Byte2BigEndian;
            else if (type == "Custom" && customType == "4 Byte Big Endian") entry.variableType = Byte4BigEndian;
            else if (type == "Custom" && customType == "Float Big Endian") entry.variableType = FloatBigEndian;
            else if (type == "String") {
                entry.variableType = String;
                if (auto* lenNode = entryNode->first_node("Length"))
                    entry.stringLength = std::stoi(lenNode->value());
            } else {
                std::cerr << "Unsupported type: " << type << " (" << customType << ")\n";
                continue;
            }

            if (auto* addressNode = entryNode->first_node("Address")) {
                std::stringstream ss;
                ss << std::hex << addressNode->value();
                ss >> entry.address;
            } else {
                std::cerr << "Missing address node in entry.\n";
                continue;
            }

            cheatEntries.push_back(entry);
        }

    } catch (std::exception& e) {
        std::cerr << "Failed to parse .CT file: " << e.what() << "\n";
        return false;
    }

    return true;
}

bool ReadValueAsFloat(pid_t pid, uintptr_t address, VariableTypes type, float& outValue) {
    switch (type) {
        case FloatBigEndian: {
            return PatchHelpers::ReadFloatBigEndian(pid, reinterpret_cast<void*>(address), outValue);
        }
        case Byte4BigEndian: {
            uint32_t val;
            if (!PatchHelpers::Read4ByteBigEndian(pid, reinterpret_cast<void*>(address), val)) return false;
            outValue = static_cast<float>(val);
            return true;
        }
        case Byte2BigEndian: {
            uint16_t val;
            if (!PatchHelpers::Read2ByteBigEndian(pid, reinterpret_cast<void*>(address), val)) return false;
            outValue = static_cast<float>(val);
            return true;
        }
        case Byte: {
            uint8_t val;
            if (!PatchHelpers::ReadByte(pid, reinterpret_cast<void*>(address), val)) return false;
            outValue = static_cast<float>(val);
            return true;
        }
        case String: {
            // Not meaningful as float; skip
            return false;
        }
    }
    return false;
}

bool WriteValueFromFloat(pid_t pid, uintptr_t address, VariableTypes type, float value, const std::string& stringValue) {
    switch (type) {
        case FloatBigEndian: {
            return PatchHelpers::WriteFloatBigEndian(pid, reinterpret_cast<void*>(address), value);
        }
        case Byte4BigEndian: {
            uint32_t val = static_cast<uint32_t>(value);
            return PatchHelpers::Write4ByteBigEndian(pid, reinterpret_cast<void*>(address), val);
        }
        case Byte2BigEndian: {
            uint16_t val = static_cast<uint16_t>(value);
            return PatchHelpers::Write2ByteBigEndian(pid, reinterpret_cast<void*>(address), val);
        }
        case Byte: {
            uint8_t val = static_cast<uint8_t>(value);
            return PatchHelpers::WriteByte(pid, reinterpret_cast<void*>(address), val);
        }
        case String: {
            if (stringValue.empty()) return false;
            return PatchHelpers::WriteString(pid, reinterpret_cast<void*>(address), stringValue.c_str(), stringValue.size());
        }
    }
    return false;
}