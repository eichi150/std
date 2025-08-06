#ifndef LISTBOX_H
#define LISTBOX_H
#include <iostream>
#include <vector>
#include <termios.h>
#include <unistd.h>

//Konsolen Listbox Auswahlfenster mit Pfeiltasten hoch und runter steuern
class Listbox{
public:
	Listbox(
		const std::vector<std::string>& options
	
	) : options(options)
	{
		this->options.push_back("Exit");
	};
	
	// Funktion zur Auswahl mit Pfeiltasten
	int select_option() {
		int current = 0;
		while (true) {
			std::cout << "\033[2J\033[1;1H"; // ANSI escape codes to clear screen and move cursor to top
			//system("clear");
			std::cout << "Wähle mit \u2191 und \u2193, Enter bestätigt:\n\n";
			for (size_t i = 0; i < options.size(); ++i) {
				if (i == current) std::cout << " > ";
				else std::cout << "   ";
				std::cout << options[i] << "\n";
			}

			char c = getch();
			if (c == '\033') { // Escape sequence
				char next = getch(); 
				if(next == '['){
					switch(getch()) {
						case 'A': // ArrowUP
							current = (current == 0) ? options.size() - 1 : current - 1;
							break;
						case 'B': // ArrowDOWN
							current = (current + 1) % options.size();
							break;
					}
				}
				
			} else if (c == '\n') {
				return current;
			}
		}
	}
	
private:
	std::vector<std::string> options;
	
	// Funktion zum Einlesen eines einzelnen Zeichens ohne Enter
	char getch() {
		struct termios oldt, newt;
		//tcgetattr(STDIN_FILENO, &oldt);
		if(tcgetattr(STDIN_FILENO, &oldt) == -1){
			throw std::runtime_error("Failed to get terminal attributes");
		}
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		//tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		if(tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1){
			throw std::runtime_error("Failed to set terminal attributes");
		}
		char c = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		return c;
	}
};

#endif // LISTBOX_H
