#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
enum logLevel { INFO, WARN, ERROR, DEBUG, FATAL ,HINT };
namespace clif
{
    inline void hideCursor() { std::cout << "\033[?25l" << std::flush; };
    inline void showCursor() { std::cout << "\033[?25h" << std::flush; };
    inline void moveCursor(const int col, const int row) { std::cout << "\033[" << col <<";"<< row << "H" << std::flush; };
    
    inline void clearScreen() { std::cout << "\033[2J\033[H" << std::flush; };


    void log(logLevel level, const std::string& msg, const int exitCode=1);
    void processing(const std::chrono::microseconds& speed);
    void loading(const int time);
    void taskbar(const std::chrono::microseconds& speed, const int width);

    std::string input(const std::string& prompt=">>");
    bool confirm(const std::string& question="");
    std::string password(const std::string& prompt="enter password:");


} // namespace cli