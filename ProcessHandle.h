#pragma once
#include "Dota2Patcher.h"
#include <tlhelp32.h>
#include <string>

class ProcessHandle {
public:
    ProcessHandle(DWORD processID, DWORD accessRights)
        : hProcess(OpenProcess(accessRights, FALSE, processID)) {
    }

    ~ProcessHandle() {
        if (hProcess) {
            CloseHandle(hProcess);
        }
    }

    ProcessHandle(const ProcessHandle&) = delete;
    ProcessHandle& operator=(const ProcessHandle&) = delete;

    ProcessHandle(ProcessHandle&& other) noexcept : hProcess(other.hProcess) {
        other.hProcess = NULL;
    }

    ProcessHandle& operator=(ProcessHandle&& other) noexcept {
        if (this != &other) {
            if (hProcess) {
                CloseHandle(hProcess);
            }
            hProcess = other.hProcess;
            other.hProcess = NULL;
        }
        return *this;
    }

    bool isValid() const {
        return hProcess != NULL;
    }

    DWORD getLastError() const {
        return hProcess ? 0 : GetLastError();
    }

    HANDLE get() const {
        return hProcess;
    }

    static DWORD GetPIDByName(const wchar_t* ProcName) {
        PROCESSENTRY32W PE32{ 0 };
        PE32.dwSize = sizeof(PE32);

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) {
            printf("[-] (ProcessHandle) CreateToolhelp32Snapshot error: 0x%d", GetLastError());
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

private:
    HANDLE hProcess;
};
