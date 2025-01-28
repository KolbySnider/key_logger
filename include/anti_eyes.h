#ifndef ANTI-EYES_H
#define ANTI-EYES_H

#include <Windows.h>
#include <tlhelp32.h>

DWORD WINAPI anti_virtual_machine(LPVOID lpParam);
DWORD WINAPI detect_process(LPVOID lpParam);

#endif
