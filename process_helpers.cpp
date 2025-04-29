#include "process_helpers.h"

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

pid_t getPidByName(const std::string& processName) {
    DIR* dir = opendir("/proc");
    if (!dir) return -1;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_DIR) continue;

        pid_t pid = atoi(entry->d_name);
        if (pid <= 0) continue;

        std::string cmdPath = "/proc/" + std::string(entry->d_name) + "/comm";
        std::ifstream cmdFile(cmdPath);
        if (!cmdFile.is_open()) continue;

        std::string name;
        std::getline(cmdFile, name);
        if (name == processName) {
            closedir(dir);
            return pid;
        }
    }

    closedir(dir);
    return -1;
}
