# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/anton/programs/clion-2018.2.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/anton/programs/clion-2018.2.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4/cmake-build-debug

# Utility rule file for UnrealWatchdog.

# Include the progress variables for this target.
include CMakeFiles/UnrealWatchdog.dir/progress.make

CMakeFiles/UnrealWatchdog:
	cd /home/anton/UnrealEngine_4.19 && bash /home/anton/UnrealEngine_4.19/Engine/Build/BatchFiles/Linux/Build.sh UnrealWatchdog Linux Development -game -progress

UnrealWatchdog: CMakeFiles/UnrealWatchdog
UnrealWatchdog: CMakeFiles/UnrealWatchdog.dir/build.make

.PHONY : UnrealWatchdog

# Rule to build all files generated by this target.
CMakeFiles/UnrealWatchdog.dir/build: UnrealWatchdog

.PHONY : CMakeFiles/UnrealWatchdog.dir/build

CMakeFiles/UnrealWatchdog.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/UnrealWatchdog.dir/cmake_clean.cmake
.PHONY : CMakeFiles/UnrealWatchdog.dir/clean

CMakeFiles/UnrealWatchdog.dir/depend:
	cd /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4 /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4 /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4/cmake-build-debug /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4/cmake-build-debug /media/anton/Kovo2/Carla/carla/Unreal/CarlaUE4/cmake-build-debug/CMakeFiles/UnrealWatchdog.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/UnrealWatchdog.dir/depend

