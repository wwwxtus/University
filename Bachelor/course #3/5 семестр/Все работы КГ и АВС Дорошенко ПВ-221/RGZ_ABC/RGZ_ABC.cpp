#include <Windows.h>
#include "peb_lookup.h"

// Define function types
typedef int (WINAPI* fpMessageBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
typedef BOOL(WINAPI* fpShellExecuteW)(HWND hWnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);

typedef int (*fp_system)(const char*);

typedef HMODULE(WINAPI* fpLoadLibraryA)(LPCSTR lpLibFileName);
typedef FARPROC(WINAPI* fpGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);

int main() {
    wchar_t k32_dll[] = { 'k','e','r','n','e','l','3','2','.','d','l','l', 0 };
    char u32_dll[] = { 'u','s','e','r','3','2','.','d','l','l', 0 };

    char load_lib[] = { 'L','o','a','d','L','i','b','r','a','r','y','A', 0 };
    char get_proc[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s', 0 };
    char msvcrt_dll[] = { 'm','s','v','c','r','t','.','d','l','l', 0 };
    char system_func[] = { 's','y','s','t','e','m', 0 };

    char msgboxw_func[] = { 'M','e','s','s','a','g','e','B','o','x','W', 0 };
    char shellexecute_func[] = { 'S','h','e','l','l','E','x','e','c','u','t','e','W', 0 };
    
    char notepad[] = { 'n','o','t', 'e', 'p', 'a', 'd', 0};

    wchar_t msg_text[] = { L'H', L'i', 0};
    wchar_t title_text[] = { L'T', L'e', L's', L't', 0};

    LPVOID kernel32_base = get_module_by_name((const LPWSTR)k32_dll);
    if (!kernel32_base) {
        return 1;
    }

    fpLoadLibraryA _LoadLibraryA = (fpLoadLibraryA)get_func_by_name((HMODULE)kernel32_base, (LPSTR)load_lib);
    fpGetProcAddress _GetProcAddress = (fpGetProcAddress)get_func_by_name((HMODULE)kernel32_base, (LPSTR)get_proc);
    if (!_LoadLibraryA || !_GetProcAddress) {
        return 2;
    }

    HMODULE user32_base = _LoadLibraryA(u32_dll);
    if (!user32_base) {
        return 5;
    }

    fpMessageBoxW _MessageBoxW = (fpMessageBoxW)_GetProcAddress(user32_base, msgboxw_func);
    if (!_MessageBoxW) {
        return 6;
    }

    HMODULE shell32_base = _LoadLibraryA(u32_dll); 
    if (!shell32_base) {
        return 7;
    }

    HMODULE msvcrt_base = _LoadLibraryA(msvcrt_dll);
    if (!msvcrt_base) {
        return 3;
    }

    int msgBoxID = _MessageBoxW(NULL, msg_text, title_text, MB_OK | MB_ICONINFORMATION);

    fp_system  _system = (fp_system)_GetProcAddress(msvcrt_base, system_func);
    
    if (msgBoxID == IDOK) {
        _system(notepad);
    }

    return 0;
}


