
compile:
	g++ ./src/main.cpp -o ./bin/std.exe
	
compile_win64:
	x86_64-w64-mingw32-g++ -std=c++17 -static -O2 -o ./win_64/std_win64.exe ./src/main.cpp

compile_win32:
	i686-w64-mingw32-g++ -std=c++17 -static -O2 -o ./win_32/std_win32.exe ./src/main.cpp

compile_ALL:
	make compile
	make compile_win32
	make compile_win64
	
main:
	micro ./src/main.cpp

install:
	./bin/install.sh
uninstall:
	./bin/uninstall.sh
update:
	source ~/.bashrc

w_install:
	micro ./bin/install.sh
w_uninstall:
	micro ./bin/uninstall.sh
	
std:
	./bin/std.exe
config:
	./bin/std.exe /home/eichi/bin/std/files /home/eichi/bin/std/files/accounts.json
