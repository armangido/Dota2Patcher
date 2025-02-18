#pragma once

#include <iostream>     // std::cout
#include <string>       // std::string
#include <format>       // std::format
#include <windows.h>    // HANDLE, GetStdHandle, SetConsoleTextAttribute

enum class CON_COLOR {
    BLACK = 0,
    CYAN = 3,
    GREEN = 10,
    BLUE = 11,
    RED = 12,
    WHITE = 15,
};

enum class CON_BG_COLOR {
    BLACK = 0x0,
    BLUE = 0x10,
    GREEN = 0x20,
    CYAN = 0x30,
    RED = 0x40,
    YELLOW = 0x60,
    WHITE = 0xF0,
};

class LOG {
public:
    template <typename... Args>
    static void DEBUG(std::format_string<Args...> format, Args&&... args) {
        log_message(CON_COLOR::WHITE, CON_BG_COLOR::BLACK, "[~]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void INFO(std::format_string<Args...> format, Args&&... args) {
        log_message(CON_COLOR::CYAN, CON_BG_COLOR::BLACK, "[i]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void ERR(std::format_string<Args...> format, Args&&... args) {
        log_message(CON_COLOR::RED, CON_BG_COLOR::BLACK, "[-]", format, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void CRITICAL(std::format_string<Args...> format, Args&&... args) {
        log_message(CON_COLOR::BLACK, CON_BG_COLOR::RED, "[!]", format, std::forward<Args>(args)...);
    }

private:
    static HANDLE hConsole;

    static void set_console_color(CON_COLOR textColor, CON_BG_COLOR bgColor) {
        if (!hConsole)
            hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(textColor) | static_cast<int>(bgColor));
    }

    template <typename... Args>
    static void log_message(CON_COLOR textColor, CON_BG_COLOR bgColor, const char* prefix, std::format_string<Args...> format, Args&&... args) {
        std::string formattedMessage = std::format(format, std::forward<Args>(args)...);
        std::string finalMessage = std::string(prefix) + " " + formattedMessage;

        set_console_color(textColor, bgColor);
        std::cout << finalMessage << std::endl;
        set_console_color(CON_COLOR::WHITE, CON_BG_COLOR::BLACK);
    }
};

inline HANDLE LOG::hConsole = nullptr;

template <typename T>
constexpr void* TO_VOID(const T& ptr) {
    return reinterpret_cast<void*>(ptr);
}
