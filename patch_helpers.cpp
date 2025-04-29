#include "patch_helpers.h"
#include <cstring>
#include <cerrno>
#include <sys/ptrace.h>
#include <endian.h>

namespace PatchHelpers {

    bool ReadMemory(pid_t pid, void* remoteAddress, void* buffer, size_t size) {
        struct iovec local[1] = { { buffer, size } };
        struct iovec remote[1] = { { remoteAddress, size } };
        ssize_t nread = process_vm_readv(pid, local, 1, remote, 1, 0);
        return (nread == static_cast<ssize_t>(size));
    }

    bool WriteMemory(pid_t pid, void* remoteAddress, const void* data, size_t size) {
        struct iovec local[1] = { { const_cast<void*>(data), size } };
        struct iovec remote[1] = { { remoteAddress, size } };
        ssize_t nwritten = process_vm_writev(pid, local, 1, remote, 1, 0);
        return (nwritten == static_cast<ssize_t>(size));
    }

    bool PatchMemoryPTRACE(pid_t pid, void* remoteAddress, const void* data, size_t size) {
        for (size_t i = 0; i < size; i += sizeof(long)) {
            long val;
            std::memcpy(&val, static_cast<const uint8_t*>(data) + i, sizeof(long));
            if (ptrace(PTRACE_POKEDATA, pid, static_cast<uint8_t*>(remoteAddress) + i, val) == -1)
                return false;
        }
        return true;
    }

    bool VerifyMemory(pid_t pid, void* remoteAddress, const void* expectedData, size_t size) {
        std::vector<uint8_t> buffer(size);
        if (!ReadMemory(pid, remoteAddress, buffer.data(), size))
            return false;
        return std::memcmp(buffer.data(), expectedData, size) == 0;
    }

    bool WriteFloatBigEndian(pid_t pid, void* remoteAddress, float value) {
        uint32_t intValue;
        std::memcpy(&intValue, &value, sizeof(float));
        intValue = htobe32(intValue);
        return WriteMemory(pid, remoteAddress, &intValue, sizeof(intValue));
    }

    bool ReadFloatBigEndian(pid_t pid, void* remoteAddress, float& outValue) {
        uint32_t intValue = 0;
        if (!ReadMemory(pid, remoteAddress, &intValue, sizeof(intValue)))
            return false;
        intValue = be32toh(intValue);
        std::memcpy(&outValue, &intValue, sizeof(float));
        return true;
    }

    bool Write4ByteBigEndian(pid_t pid, void* remoteAddress, uint32_t value) {
        uint32_t bigEndianValue = htobe32(value);
        return PatchHelpers::WriteMemory(pid, remoteAddress, &bigEndianValue, sizeof(bigEndianValue));
    }

    bool Read4ByteBigEndian(pid_t pid, void* remoteAddress, uint32_t& outValue) {
        uint32_t temp;
        if (!PatchHelpers::ReadMemory(pid, remoteAddress, &temp, sizeof(temp)))
            return false;
        outValue = be32toh(temp);
        return true;
    }

    bool Write2ByteBigEndian(pid_t pid, void* remoteAddress, uint16_t value) {
        uint16_t bigEndianValue = htobe16(value);
        return PatchHelpers::WriteMemory(pid, remoteAddress, &bigEndianValue, sizeof(bigEndianValue));
    }

    bool Read2ByteBigEndian(pid_t pid, void* remoteAddress, uint16_t& outValue) {
        uint16_t temp;
        if (!PatchHelpers::ReadMemory(pid, remoteAddress, &temp, sizeof(temp)))
            return false;
        outValue = be16toh(temp);
        return true;
    }

    bool WriteByte(pid_t pid, void* remoteAddress, uint8_t value) {
        return WriteMemory(pid, remoteAddress, &value, 1);
    }
    
    bool ReadByte(pid_t pid, void* remoteAddress, uint8_t& outValue) {
        uint8_t byte = 0;
        if (!ReadMemory(pid, remoteAddress, &byte, 1))
            return false;
        outValue = byte;
        return true;
    }

    bool WriteString(pid_t pid, void* remoteAddress, const char* in, size_t length) {
        if (length > 256) length = 256; // cap for safety, can be adjusted
        return WriteMemory(pid, remoteAddress, in, length);
    }


    bool ReadString(pid_t pid, void* remoteAddress, char* out, size_t length) {
        if (length > 256) length = 256; // cap for safety, can be adjusted
        char buffer[256] = { 0 };
        if (!ReadMemory(pid, remoteAddress, buffer, length))
            return false;
        std::memcpy(out, buffer, length);
        return true;
    }

}