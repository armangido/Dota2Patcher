#pragma once

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <Windows.h>

enum class ConColor {
    BLACK = 0,
    CYAN = 3,
    GREEN = 10,
    BLUE = 11,
    RED = 12,
    WHITE = 15,
};

enum class ConBackgroundColor {
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
    static void DEBUG(const char* format, ...) {
        va_list arglist;
        va_start(arglist, format);
        LogMessage(ConColor::WHITE, ConBackgroundColor::BLACK, "[~]", format, arglist);
        va_end(arglist);
    }

    static void INFO(const char* format, ...) {
        va_list arglist;
        va_start(arglist, format);
        LogMessage(ConColor::CYAN, ConBackgroundColor::BLACK, "[i]", format, arglist);
        va_end(arglist);
    }

    static void ERR(const char* format, ...) {
        va_list arglist;
        va_start(arglist, format);
        LogMessage(ConColor::RED, ConBackgroundColor::BLACK, "[-]", format, arglist);
        va_end(arglist);
    }

    static void CRITICAL(const char* format, ...) {
        va_list arglist;
        va_start(arglist, format);
        LogMessage(ConColor::BLACK, ConBackgroundColor::RED, "[!]", format, arglist);
        va_end(arglist);
    }

private:
    static HANDLE hConsole;

    static void SetConsoleColor(ConColor textColor, ConBackgroundColor bgColor) {
        if (!hConsole)
            hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(textColor) | static_cast<int>(bgColor));
    }

    static void LogMessage(ConColor textColor, ConBackgroundColor bgColor, const char* prefix, const char* format, va_list args) {
        va_list args_copy;
        va_copy(args_copy, args);
        int size = std::vsnprintf(nullptr, 0, format, args_copy);
        va_end(args_copy);

        if (size < 0) 
            return;

        string buf(size, '\0');
        std::vsnprintf(buf.data(), buf.size() + 1, format, args);
        string finalMessage = string(prefix) + " " + buf;

        SetConsoleColor(textColor, bgColor);
        cout << finalMessage << endl;
        SetConsoleColor(ConColor::WHITE, ConBackgroundColor::BLACK);
    }
};

inline HANDLE LOG::hConsole = nullptr;
