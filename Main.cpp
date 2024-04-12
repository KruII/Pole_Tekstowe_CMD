#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <sstream>
#include <vector>

void SetCursorPosition(int x, int y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Position = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hOut, Position);
}

void SetTextAttribute(WORD attribute) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut, attribute);
}

int GetConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

void DrawBox(int lines, int maxWidth) {
    int consoleWidth = GetConsoleWidth();
    int boxWidth = maxWidth + 4; // plus margins for the text
    int startX = (consoleWidth - boxWidth) / 2;
    int startY = 1; // Start drawing box at line 1

    SetTextAttribute(FOREGROUND_RED); // Red box
    for (int i = 0; i <= lines + 2; ++i) {
        SetCursorPosition(startX, startY + i);
        if (i == 0) {
            std::cout << char(201); // Left corner
            for (int j = 0; j < boxWidth - 2; ++j) std::cout << char(205); // Horizontal line
            std::cout << char(187); // Right corner
        }
        else {
            std::cout << char(186); // Vertical line
            SetCursorPosition(startX + boxWidth - 1, startY + i);
            std::cout << char(186); // Vertical line
        }
    }
    SetCursorPosition(startX, startY + lines + 3);
    std::cout << char(200);  // Left bottom corner
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << char(205);  // Bottom horizontal line
    std::cout << char(188);  // Right bottom corner
}

void DisplayText(const std::vector<std::string>& lines, int maxWidth, int cursorX, int cursorY) {
    system("cls"); // Clear the console
    int consoleWidth = GetConsoleWidth();
    int startX = (consoleWidth - maxWidth) / 2 + 2; // Adjust for box left margin

    SetTextAttribute(FOREGROUND_GREEN); // Green text
    for (int i = 0; i < lines.size(); ++i) {
        SetCursorPosition(startX, 3 + i); // Adjust for box top margin
        std::cout << lines[i];
    }

    DrawBox(max(1, static_cast<int>(lines.size())), maxWidth); // Ensure minimum height of 3 lines
    SetCursorPosition(startX + cursorX, 3 + cursorY); // Adjust cursor position for box margins
}

int main() {
    std::vector<std::string> lines = { "" };
    char ch;
    int maxWidth = 50; // Maximum width of the text box
    int cursorX = 0, cursorY = 0;

    while (true) {
        if (_kbhit()) {
            ch = _getch();
            if (ch == 27) { // ESC key to exit
                break;
            }
            else if (ch == 13) { // Enter key
                std::string currentLine = lines[cursorY];
                std::string newLine = currentLine.substr(cursorX);
                currentLine = currentLine.substr(0, cursorX);
                lines[cursorY] = currentLine;
                lines.insert(lines.begin() + cursorY + 1, newLine);
                cursorY++;
                cursorX = 0;
            }
            else if (ch == 8) { // Backspace key
                if (cursorX > 0) {
                    lines[cursorY].erase(cursorX - 1, 1);
                    cursorX--;
                }
                else if (cursorY > 0) {
                    cursorX = lines[cursorY - 1].length();
                    lines[cursorY - 1] += lines[cursorY];
                    lines.erase(lines.begin() + cursorY);
                    cursorY--;
                }
            }
            else if (ch == 9) { // Tab key
                int i = 0;
                while (i < 4 && cursorX < maxWidth - 2)
                {
                    i++;
                    lines[cursorY].insert(cursorX, " ");
                    cursorX++;
                }
            }
            else if (ch == -32) { // Arrow keys (special keys)
                ch = _getch();
                if (ch == 75 && cursorX > 0) { // Left arrow
                    cursorX--;
                }
                else if (ch == 77 && cursorX < lines[cursorY].length()) { // Right arrow
                    cursorX++;
                }
                else if (ch == 72 && cursorY > 0) { // Up arrow
                    cursorY--;
                    cursorX = min(cursorX, static_cast<int>(lines[cursorY].length()));
                }
                else if (ch == 80 && cursorY < lines.size() - 1) { // Down arrow
                    cursorY++;
                    cursorX = min(cursorX, static_cast<int>(lines[cursorY].length()));
                }
                else if (ch == 75 && cursorX == 0 && cursorY > 0) {
                    cursorY--;
                    cursorX = lines[cursorY].length();
                }
                else if (ch == 77 && cursorX == lines[cursorY].length() && cursorY < lines.size()-1) {
                    cursorY++;
                    cursorX = 0;
                }
            }
            else if (cursorX < maxWidth-2) { // Printable characters
                lines[cursorY].insert(cursorX, 1, ch);
                cursorX++;
            }
            DisplayText(lines, maxWidth, cursorX, cursorY);
        }
    }

    SetTextAttribute(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Resetting color to default
    return 0;
}
