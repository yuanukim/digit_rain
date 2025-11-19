/*
    a c++ program to simulate digital rain on your console. for windows platform only.
    this program uses some windows system calls, but they seldom fail, so the return 
    value checking is just ignored.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <array>
#include <cstdint>

// a class to handle the console stuff.
class Console {
    HANDLE outputHandle;
    WORD oldColorAttribute;

    Console();
public:
    void clear_screen();
    void set_cursor_pos(short x, short y);
    void enable_cursor_shown(bool flag);
    void set_text_color(int colorAttibute);
    void set_default_text_color();

    static Console& instance();
};

struct Rain {
    int row;
    int col;
    char c;
};

// callback to handle Ctrl + C signal on windows platform.
BOOL WINAPI console_signal_handler(DWORD signal);

// show it.
void show_digital_rain();

Console::Console() {
    outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(outputHandle, &csbi);
    oldColorAttribute = csbi.wAttributes;
}

void Console::clear_screen() {
    system("cls");
}

void Console::set_cursor_pos(short x, short y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(outputHandle, pos);
}

void Console::enable_cursor_shown(bool flag) {
    CONSOLE_CURSOR_INFO cci;
    
    GetConsoleCursorInfo(outputHandle, &cci);
    cci.bVisible = (flag ? 1 : 0);
    SetConsoleCursorInfo(outputHandle, &cci);
}

void Console::set_text_color(int colorAttibute) {
    SetConsoleTextAttribute(outputHandle, colorAttibute);
}

void Console::set_default_text_color() {
    SetConsoleTextAttribute(outputHandle, oldColorAttribute);
}

Console& Console::instance() {
    static Console c;
    return c;
}

BOOL WINAPI console_signal_handler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        Console::instance().enable_cursor_shown(true);
        Console::instance().set_default_text_color();
        Console::instance().clear_screen();
        ExitProcess(0);
    }

    return true;
}

void show_digital_rain() {
    const int height = 25;
    const int width = 80;
    const int delay_milli_sec = 200;
    const int bold_green_color_attr = 0xa;

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    auto mt = std::mt19937_64{ static_cast<uint64_t>(seed) };
    auto dist = std::uniform_int_distribution<int>(0, 65536);

    std::array<Rain, 100> rain;

    for (Rain& r : rain) {
        r.row = dist(mt) % height;
        r.col = dist(mt) % width;
        r.c = dist(mt) % 2 + '0';
    }

    std::array<std::array<char, width + 1>, height> buffer;

    for (auto& line : buffer) {
        line.fill(' ');
        line[width] = '\n';
    }

    Console::instance().set_text_color(bold_green_color_attr);

    while (true) {
        for (Rain& r : rain) {
            buffer[r.row - 5][r.col] = ' ';
            buffer[r.row][r.col] = r.c;

            r.row += 1;

            if (r.row == height) {
                buffer[r.row - 5][r.col] = ' ';
                buffer[r.row - 4][r.col] = ' ';
                buffer[r.row - 3][r.col] = ' ';
                buffer[r.row - 2][r.col] = ' ';
                buffer[r.row - 1][r.col] = ' ';

                r.row = dist(mt) % height;
                r.col = dist(mt) % width;
                r.c = dist(mt) % 2 + '0';
            }

            Console::instance().set_cursor_pos(0, 0);
            std::cout.write(&(buffer[0][0]), height * (width + 1));
            std::cout.flush();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_milli_sec));
    }
}

int main() {
    SetConsoleCtrlHandler(console_signal_handler, true);

    Console::instance().clear_screen();
    Console::instance().enable_cursor_shown(false);

    show_digital_rain();
    return 0;
}
