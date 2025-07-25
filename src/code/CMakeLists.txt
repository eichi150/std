cmake_minimum_required(VERSION 3.10)
project(std_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
	enum_class.cpp
	time_account.cpp
	json_handler.cpp
	translator.cpp
	arg_manager.cpp
	main.cpp
)
# Default build (compile): für Host-System (z. B. Linux)
add_executable(std ${SOURCES})

set_target_properties(std PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/linux)

# Zielverzeichnisse anlegen, falls nicht vorhanden
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/linux)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/win_32)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/win_64)

# Ziel compile_win32 (cross-compile)
add_custom_target(compile_win32
    COMMAND i686-w64-mingw32-g++ -std=c++17 -static -O2 -o ${CMAKE_SOURCE_DIR}/win_32/std_win32.exe ${SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building Windows 32-bit binary"
)

# Ziel compile_win64 (cross-compile)
add_custom_target(compile_win64
    COMMAND x86_64-w64-mingw32-g++ -std=c++17 -static -O2 -o ${CMAKE_SOURCE_DIR}/win_64/std_win64.exe ${SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building Windows 64-bit binary"
)

# compile_ALL: entspricht dem Makefile
add_custom_target(compile_ALL
    DEPENDS std compile_win32 compile_win64
    COMMENT "Building all targets"
)
