#include "utils.h"
#include <iostream>
#include <cstdlib>

//takes from stackoverflow//
void gotoxy(int x, int y) {
    std::cout.flush();                                  // Make sure all previous output is printed
    COORD coordinate;                                   // Windows console coordinate structure
    coordinate.X = x;                                   // Set column
    coordinate.Y = y;                                   // Set row
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate); // Move cursor
}

void cls()
{
    system("cls()");                                    // Clear the console screen (Windows command)
}

//ChatGPT//
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); // Get handle to console output
    CONSOLE_CURSOR_INFO info;                              // Struct that holds cursor settings
    info.bVisible = FALSE;                                 // Hide the blinking text cursor
    info.dwSize = 100;                                     // Cursor size (100% of character cell)
    SetConsoleCursorInfo(consoleHandle, &info);            // Apply new cursor settings
}
