#ifndef KEY_HOOK_H
#define KEY_HOOK_H

#include <Windows.h>
#include <string>


LRESULT keyboard_proc(int nCode, WPARAM wparam, LPARAM lparam);
bool install_hook();
bool uninstall_hook();
bool is_hooked();

#endif
