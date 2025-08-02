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
		std::vector<std::string>& options
	
	) : options(options)
	{
		options.push_back("Exit");
	};
		
	~Listbox(){
		options.pop_back();
	}
	// Funktion zur Auswahl mit Pfeiltasten
	int select_option() {
		int current = 0;
		while (true) {
			system("clear");
			std::cout << "Wähle mit \u2191 und \u2193, Enter bestätigt:\n\n";
			for (size_t i = 0; i < options.size(); ++i) {
				if (i == current) std::cout << " > ";
				else std::cout << "   ";
				std::cout << options[i] << "\n";
			}

			char c = getch();
			if (c == '\033') { // Escape sequence
				getch(); // skip [
				switch(getch()) {
					case 'A': // ArrowUP
						current = (current == 0) ? options.size() - 1 : current - 1;
						break;
					case 'B': // ArrowDOWN
						current = (current + 1) % options.size();
						break;
				}
			} else if (c == '\n') {
				return current;
			}
		}
	}
	
private:
	std::vector<std::string>& options;
	
	// Funktion zum Einlesen eines einzelnen Zeichens ohne Enter
	char getch() {
		struct termios oldt, newt;
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		char c = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		return c;
	}
};

#endif // LISTBOX_H
