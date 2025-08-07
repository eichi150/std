cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0079 NEW)

project(std_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Replxx einbinden
add_subdirectory(interface/extern/replxx)
set_target_properties(replxx PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

set(SOURCES
    main.cpp
    master.cpp
    ./control/cmd_ctrl.cpp
    time_account.cpp
    json_handler.cpp
    ./manager/arg_manager.cpp
    ./manager/env_manager.cpp
    ./commands/command.cpp
    ./commands/change_command.cpp
    ./commands/show_command.cpp
    ./commands/alias_command.cpp
    ./commands/add_alias_command.cpp
    ./commands/entity_command.cpp
    ./interface/translator.cpp
    ./interface/cli_ui.cpp
    ./interface/replex.cpp
)

if(UNIX AND NOT APPLE)
    message(STATUS "Compiling for Linux")
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/bme280/bme280_sensor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/bme280/bme280.c
        ${CMAKE_CURRENT_SOURCE_DIR}/control/device_ctrl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/control/device_processor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/commands/handle_crontab.cpp
    )
endif()

add_executable(std ${SOURCES})

target_include_directories(std PRIVATE interface/extern/replxx/include)

if(UNIX AND NOT APPLE)
    target_include_directories(std PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/bme280)
endif()

find_package(Threads REQUIRED)

target_link_libraries(std PRIVATE replxx Threads::Threads)

# Setze Threadflags, falls nötig (bei POSIX-Threads mit MinGW-w64-Posix sollte das automatisch sein)
set_target_properties(std PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Output-Verzeichnisse
set_target_properties(std PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/linux)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/linux)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/win_32)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/win_64)

# Windows-spezifische Quellen (ohne Linux-spezifische Dateien)
set(WINDOWS_SOURCES
    main.cpp
    master.cpp
    ./control/cmd_ctrl.cpp
    time_account.cpp
    json_handler.cpp
    ./manager/arg_manager.cpp
    ./manager/env_manager.cpp
    ./commands/command.cpp
    ./commands/change_command.cpp
    ./commands/show_command.cpp
    ./commands/alias_command.cpp
    ./commands/add_alias_command.cpp
    ./commands/entity_command.cpp
    ./interface/translator.cpp
    ./interface/cli_ui.cpp
    ./interface/replex.cpp
)

# Cross-Compile Targets (kann man je nach Bedarf anpassen)
add_custom_target(compile_win32
    COMMAND i686-w64-mingw32-g++ -std=c++17 -static -O2 -o ${CMAKE_SOURCE_DIR}/win_32/std_win32.exe ${WINDOWS_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building Windows 32-bit binary"
)

add_custom_target(compile_ALL
    DEPENDS std compile_win32
    COMMENT "Building all targets"
)
