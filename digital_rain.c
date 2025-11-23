/*
    a c program to simulate digital rain on your console, only for windows platform.

    because the system calls in this program won't fail in most scenarios, so I don't really 
    check their return values.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIGITAL_RAIN_CHARACTERS_NUM  100
#define HEIGHT  25
#define WIDTH   80
#define DELAY_MILLI_SEC   200
#define COLOR_BOLD_GREEN  0xa

struct Rain {
    int row;
    int col;
    char c;
};

static HANDLE outputHandle = INVALID_HANDLE_VALUE;
static WORD oldConsoleColor;
static struct Rain rain[DIGITAL_RAIN_CHARACTERS_NUM];

BOOL WINAPI console_signal_handler(DWORD signal);
void init(void);
void reset(void);
void console_clear_screen(void);
void console_set_cursor_pos(SHORT x, SHORT y);
void console_enable_cursor_shown(int enable);
void console_set_text_color(int color);
void show_digital_rain(void);

void init(void) {
    int i;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    GetConsoleScreenBufferInfo(outputHandle, &csbi);
    oldConsoleColor = csbi.wAttributes;

    console_enable_cursor_shown(0);
    srand(time(NULL));

    for (i = 0; i < DIGITAL_RAIN_CHARACTERS_NUM; ++i) {
        rain[i].col = rand() % WIDTH;
        rain[i].row = rand() % HEIGHT;
        rain[i].c = rand() % 2 + '0';
    }
}

BOOL WINAPI console_signal_handler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        reset();
        console_clear_screen();
        ExitProcess(0);
    }

    return TRUE;
}

void console_clear_screen(void) {
    system("cls");
}

void reset(void) {
    console_enable_cursor_shown(1);
    SetConsoleTextAttribute(outputHandle, oldConsoleColor);
}

void console_set_cursor_pos(SHORT x, SHORT y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(outputHandle, pos);
}

void console_enable_cursor_shown(int enable) {
    CONSOLE_CURSOR_INFO cci;
    
    GetConsoleCursorInfo(outputHandle, &cci);
    cci.bVisible = enable;
    SetConsoleCursorInfo(outputHandle, &cci);
}

void console_set_text_color(int color) {
    SetConsoleTextAttribute(outputHandle, color);
}

void show_digital_rain(void) {
    int i, j;
    char buffer[HEIGHT][WIDTH + 1];

    for (i = 0; i < HEIGHT; ++i) {
        for (j = 0; j < WIDTH + 1; ++j) {
            if (j == WIDTH) {
                buffer[i][j] = '\n';
            }
            else {
                buffer[i][j] = ' ';
            }
        }
    }

    console_set_text_color(COLOR_BOLD_GREEN);

    while (1) {
        for (i = 0; i < DIGITAL_RAIN_CHARACTERS_NUM; ++i) {
            buffer[rain[i].row - 5][rain[i].col] = ' ';
            buffer[rain[i].row][rain[i].col] = rain[i].c;

            rain[i].row += 1;

            if (rain[i].row == HEIGHT) {
                buffer[rain[i].row - 5][rain[i].col] = ' ';
                buffer[rain[i].row - 4][rain[i].col] = ' ';
                buffer[rain[i].row - 3][rain[i].col] = ' ';
                buffer[rain[i].row - 2][rain[i].col] = ' ';
                buffer[rain[i].row - 1][rain[i].col] = ' ';

                rain[i].col = rand() % WIDTH;
                rain[i].row = rand() % HEIGHT;
                rain[i].c = rand() % 2 + '0';
            }
        }

        console_set_cursor_pos(0, 0);
        fwrite(buffer, sizeof(char), HEIGHT * (WIDTH + 1) * sizeof(char), stdout);
        fflush(stdout);
        Sleep(DELAY_MILLI_SEC);
    }
}

int main(void) {
    SetConsoleCtrlHandler(console_signal_handler, TRUE);
    console_clear_screen();
    init();
    show_digital_rain();
    return 0;
}
