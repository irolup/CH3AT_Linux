#ifndef PROCESS_HELPERS_H
#define PROCESS_HELPERS_H

#include <string>
#include <vector>
#include <cstdint>

pid_t getPidByName(const std::string& processName);

#endif // PROCESS_HELPERS_H