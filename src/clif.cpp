#include "../include/clif.h"
namespace clif
{
    
    void log(logLevel level, const std::string& msg, const int exitCode) {
        switch (level) {
            case INFO:
                std::cout << "\033[3;1;32minfo\033[0m: " << msg << "\n";
                break; 
            case WARN:
                std::cout << "\033[3;1;33mwarning\033[0m: " << msg << "\n";
                break; 
            case ERROR:
                std::cout << "\033[3;1;31merror\033[0m: " << msg << "\n";
                break;
            case FATAL:
                std::cout << "\033[3;1;31mfatal\033[0m: " << msg << "\n";
                if (exitCode != -1){exit(exitCode);}
                break; 
            case DEBUG:
                std::cout << "\033[3;1;36mdebug\033[0m: " << msg << "\n";
                break;  
            case HINT:
                std::cout << "\033[3;1;33mhint: " << msg << "\033[0m\n";
                break; 
            case NONE:
                std::cout << msg << std::endl;
                break;  
        }
    }
    void echo(const bool enable) {
        termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        if (enable)
            tty.c_lflag |= ECHO;
        else
            tty.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    void processing(const std::chrono::microseconds& speed){
        echo(false); 
        hideCursor();
        for (int i = 0; i <= 100; i++) {
            std::cout << "\r" << i << "%" << std::flush;
            std::this_thread::sleep_for(speed);
        }
        std::cout << "\n";
        echo(true); 
        showCursor();
    }
    void loading(int time){
        echo(false); 
        hideCursor();
        char states[] = {'|', '/', '-', '\\'};
        for (int i = 0; i <= time; i++) {
            std::cout << "\r" << states[i % 4] << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(120));
        }
        std::cout << "\n";
        echo(true); 
        showCursor();

    }

    void taskbar(const std::chrono::microseconds& speed ,const int width){
        echo(false); 

        hideCursor();

        for (int j = 0; j <= width; j++){ 
            std::cout << "\033[1;0;100m "<< std::flush;
        }
        std::cout << "\r"<< std::flush;
        for (int i = 0; i <= width; i++) {
            std::cout << "\033[1;0;44m "<< std::flush;
            std::this_thread::sleep_for(speed);
        }
        std::cout << "\033[0m\n";
        echo(true); 
        showCursor();

    }

    std::string input(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::cin >> input;
        return input;
    }

    std::string password(const std::string& prompt) {
        std::cout << prompt;
        std::cout.flush();

        echo(false); 

        std::string input;
        char c;
        while (true) {
            c = getchar();
            if (c == '\n' || c == '\r') break;
            input += c;
        }

        echo(true); 
        std::cout << std::endl;
        return input;
    }

    bool confirm(const std::string& question){
        std::string answer;
        std::cout << question << " [Y/n]: ";
        std::cin >> answer;
        if (std::tolower(answer[0])=='y'){return true;} 
        return false;
    }

} // namespace cli