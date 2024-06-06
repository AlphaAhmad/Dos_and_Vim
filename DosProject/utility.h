#pragma once
#ifndef UTILITY
#define UTILITY
#include <iostream>
#include <vector>
#include <conio.h>
#include <unordered_map>
#include <map>
#include <sstream>
#include <fstream>
#include <list>
#include <algorithm>
#include <Windows.h>
#include <queue>
#include <string>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15

class ConsoleHandler
{
public:
    static void SetClr(int tcl, int bcl)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (tcl + (bcl * 16)));
    }

    static void gotoRowCol(int rpos, int cpos)
    {
        COORD scrn;
        HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
        scrn.X = cpos;
        scrn.Y = rpos;
        SetConsoleCursorPosition(hOuput, scrn);
    }

    static void getConsoleSize(int &columns, int &rows)
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
};

// ++++++   (just a virtual class to excess other directories in the same map) ++++++++
class directory_items
{
    char x;

public:
    directory_items()
    {
        x = ' ';
    }
    virtual ~directory_items() = default;
};

#endif