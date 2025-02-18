#pragma once

class Utils {
public:
    static optional<string> wchar_to_string(const wchar_t* wideStr) {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0)
            return nullopt;

        string result(size_needed - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &result[0], size_needed, nullptr, nullptr);
        return result;
    }

    static std::wstring string_to_wstring(const string& str) {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
        return wstr;
    }
};

