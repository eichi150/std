compile:
	g++ ./src/main.cpp -o ./bin/std.exe
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
