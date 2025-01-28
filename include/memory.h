#ifndef MEMORY_H
#define MEMORY_H
#include <mutex>
#include <vector>
#include <Windows.h>

// Global (not implemented) encrypted log buffer + mutex for thread safety
inline std::vector<BYTE> g_plainLogs;
inline std::mutex g_logMutex;


// so i can email with ps unsafe but too lazy to implement proper smtp
void save_logs_to_file();
void send_email();

#endif //MEMORY_H
