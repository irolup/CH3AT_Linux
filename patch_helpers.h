#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <sys/uio.h>
#include <unistd.h>

namespace PatchHelpers {

    bool ReadMemory(pid_t pid, void* remoteAddress, void* buffer, size_t size);
    bool WriteMemory(pid_t pid, void* remoteAddress, const void* data, size_t size);
    bool PatchMemoryPTRACE(pid_t pid, void* remoteAddress, const void* data, size_t size);
    bool VerifyMemory(pid_t pid, void* remoteAddress, const void* expectedData, size_t size);
    
    bool WriteFloatBigEndian(pid_t pid, void* remoteAddress, float value);
    bool ReadFloatBigEndian(pid_t pid, void* remoteAddress, float& outValue);
    
    bool Write4ByteBigEndian(pid_t pid, void* remoteAddress, uint32_t value);
    bool Read4ByteBigEndian(pid_t pid, void* remoteAddress, uint32_t& outValue);
    
    bool Write2ByteBigEndian(pid_t pid, void* remoteAddress, uint16_t value);
    bool Read2ByteBigEndian(pid_t pid, void* remoteAddress, uint16_t& outValue);
    
    bool WriteByte(pid_t pid, void* remoteAddress, uint8_t value);
    bool ReadByte(pid_t pid, void* remoteAddress, uint8_t& outValue);
    
    bool WriteString(pid_t pid, void* remoteAddress, const char* str, size_t length);
    bool ReadString(pid_t pid, void* remoteAddress, char* buffer, size_t length);

}