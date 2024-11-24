# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = /snap/cmake/1429/bin/cmake

# The command to remove a file.
RM = /snap/cmake/1429/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/adit/vdi/csci-598j-project3/rft-client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/adit/vdi/csci-598j-project3

# Utility rule file for submit.

# Include any custom commands dependencies for this target.
include CMakeFiles/submit.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/submit.dir/progress.make

CMakeFiles/submit:
	/snap/cmake/1429/bin/cmake -E echo Getting\ the\ current\ username...
	/snap/cmake/1429/bin/cmake -E echo Username:\ croutsis
	/snap/cmake/1429/bin/cmake -E echo Creating\ directory\ named\ after\ the\ user...
	/snap/cmake/1429/bin/cmake -E make_directory /mnt/adit/vdi/csci-598j-project3/croutsis
	/snap/cmake/1429/bin/cmake -E echo Copying\ source\ files\ to\ the\ users\ directory...
	/snap/cmake/1429/bin/cmake -E copy_if_different /mnt/adit/vdi/csci-598j-project3/rft-client/unreliableTransport.cpp /mnt/adit/vdi/csci-598j-project3/rft-client/unreliableTransport.h /mnt/adit/vdi/csci-598j-project3/rft-client/main.cpp /mnt/adit/vdi/csci-598j-project3/rft-client/logging.h /mnt/adit/vdi/csci-598j-project3/rft-client/datagram.cpp /mnt/adit/vdi/csci-598j-project3/rft-client/datagram.h /mnt/adit/vdi/csci-598j-project3/rft-client/timerC.cpp /mnt/adit/vdi/csci-598j-project3/rft-client/timerC.h /mnt/adit/vdi/csci-598j-project3/croutsis
	/snap/cmake/1429/bin/cmake -E echo Creating\ tarball\ of\ the\ directory...
	/snap/cmake/1429/bin/cmake -E tar czf /mnt/adit/vdi/csci-598j-project3/rft-client/croutsis.tar.gz /mnt/adit/vdi/csci-598j-project3/croutsis
	/snap/cmake/1429/bin/cmake -E echo Submission\ tarball\ created:\ /mnt/adit/vdi/csci-598j-project3/rft-client/croutsis.tar.gz

CMakeFiles/submit.dir/codegen:
.PHONY : CMakeFiles/submit.dir/codegen

submit: CMakeFiles/submit
submit: CMakeFiles/submit.dir/build.make
.PHONY : submit

# Rule to build all files generated by this target.
CMakeFiles/submit.dir/build: submit
.PHONY : CMakeFiles/submit.dir/build

CMakeFiles/submit.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/submit.dir/cmake_clean.cmake
.PHONY : CMakeFiles/submit.dir/clean

CMakeFiles/submit.dir/depend:
	cd /mnt/adit/vdi/csci-598j-project3 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/adit/vdi/csci-598j-project3/rft-client /mnt/adit/vdi/csci-598j-project3/rft-client /mnt/adit/vdi/csci-598j-project3 /mnt/adit/vdi/csci-598j-project3 /mnt/adit/vdi/csci-598j-project3/CMakeFiles/submit.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/submit.dir/depend

