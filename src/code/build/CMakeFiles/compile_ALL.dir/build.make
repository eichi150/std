# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/eichi/Dev/Projekte/std/src/code

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/eichi/Dev/Projekte/std/src/code/build

# Utility rule file for compile_ALL.

# Include any custom commands dependencies for this target.
include CMakeFiles/compile_ALL.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/compile_ALL.dir/progress.make

CMakeFiles/compile_ALL: /home/eichi/Dev/Projekte/std/src/code/linux/std
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/eichi/Dev/Projekte/std/src/code/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building all targets"

compile_ALL: CMakeFiles/compile_ALL
compile_ALL: CMakeFiles/compile_ALL.dir/build.make
.PHONY : compile_ALL

# Rule to build all files generated by this target.
CMakeFiles/compile_ALL.dir/build: compile_ALL
.PHONY : CMakeFiles/compile_ALL.dir/build

CMakeFiles/compile_ALL.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/compile_ALL.dir/cmake_clean.cmake
.PHONY : CMakeFiles/compile_ALL.dir/clean

CMakeFiles/compile_ALL.dir/depend:
	cd /home/eichi/Dev/Projekte/std/src/code/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/eichi/Dev/Projekte/std/src/code /home/eichi/Dev/Projekte/std/src/code /home/eichi/Dev/Projekte/std/src/code/build /home/eichi/Dev/Projekte/std/src/code/build /home/eichi/Dev/Projekte/std/src/code/build/CMakeFiles/compile_ALL.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/compile_ALL.dir/depend

