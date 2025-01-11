#pragma once
#include "Dota2Patcher.h"
#include <tlhelp32.h>
#include <string>

class ProcessHandle {
public:
    static DWORD get_PID_by_name(const wchar_t* ProcName) {
        PROCESSENTRY32W PE32{ 0 };
        PE32.dwSize = sizeof(PE32);

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) {
            printf("[-] (ProcessHandle) CreateToolhelp32Snapshot error: 0x%d\n", GetLastError());
            return 0;
        }

        DWORD PID = 0;
        BOOL bRet = Process32FirstW(hSnap, &PE32);
        while (bRet) {
            if (wcscmp(ProcName, PE32.szExeFile) == 0) {
                PID = PE32.th32ProcessID;
                break;
            }

            bRet = Process32NextW(hSnap, &PE32);
        }

        CloseHandle(hSnap);
        return PID;
    }

    static void open_process_handle(DWORD processID, DWORD accessRights) {
        if (hProcess != NULL)
            CloseHandle(hProcess);

        hProcess = OpenProcess(accessRights, FALSE, processID);
    }

    static void close_process_handle() {
        if (hProcess) {
            CloseHandle(hProcess);
            hProcess = NULL;
        }
    }

    static bool is_valid_handle() {
        return hProcess != NULL;
    }

    static DWORD get_last_error() {
        return hProcess ? 0 : ::GetLastError();
    }

    static HANDLE get_handle() {
        return hProcess;
    }

private:
    static inline HANDLE hProcess = NULL;
};