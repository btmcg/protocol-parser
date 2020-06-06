#￼Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


# Example Module.mk

# The bare-minimum required for a Module.mk file is to define the
# type of module to be built (executable, shared library, or static
# library) and provide the path. The following line is all that is
# required to create a c or c++ executable.
#
#     $(call add-executable-module,$(call get-path))
#
# for a shared library:
#
#     $(call add-shared-library-module,$(call get-path))
#
# and for a static library:
#
#     $(call add-static-library-module,$(call get-path))


# Acceptable variables:

# LOCAL_CFLAGS
# Flags to be passed to the C compiler. This should contain any CFLAGS
# necessary for building this particular module. Only necessary for C
# programs.
# Example: -std=c11
# Default: empty

# LOCAL_CPPFLAGS
# Flags to be passed to the C preprocessor and programs that use it.
# This should contains any CPPFLAGS necessary for building this
# particular module. Relevant for C and C++ programs.
# Example: -ggdb3 -MMD -O3
# Defualt: empty

# LOCAL_CXXFLAGS
# Flags to be passed to the C++ compiler. This should contain any
# CXXFLAGS necessary for building this particular module. Only necessary
# for C++ programs.
# Example: -std=c++17
# Default: empty

# LOCAL_DEPS
# This should contain any additional dependencies (internal to the
# module) required to successfully compile the c/c++ source code listed
# in LOCAL_SOURCE_FILES.
# Example: extra_file.h extra_file2.h
# Default: <file>.d dependency files created by using compiler features
# such as -MMD.

# LOCAL_LDFLAGS
# Flags to be passed to the linker. This should contain any LDFLAGS
# necessary for linking this particular module.
# Example: -L/path/to/lib1 -L/path/to/lib2
# Default: empty

# LOCAL_LDLIBS
# Library names and flags to be passed to the linker. This should
# contain any LDLIBS necessary for linking this particular module.
# Example: -pthread -lmylib1 -lmylib2
# Default: empty

# LOCAL_LIBRARIES
# This should contain the basename of libraries within the project that
# this module depends on. For example, if this module relies on the
# module defined in src/mylib/Module.mk, then this variable should
# contain "mylib".
# Example: core mylib1 mylib2
# Default: empty

# LOCAL_MODULE
# This should contain the name of this particular module, if it should
# be different than the basename of the path defined by calling
# get-path.
# Example: mymodule
# Default: basename of $(call get-path)

# LOCAL_OBJS
# This should contain the names of the object files to be created as a
# result of the compilation process.
# Example: file1.o file2.o
# Default: LOCAL_SOURCE_FILES with extensions replaced by 'o'.

# LOCAL_SOURCE_FILES
# This should contain the name of all source files to be compiled.
# Example: $(call rwildcard,$(call get-path),*.cpp) which will populate
# the variable with all cpp files in every subdirectory of the module.
# Default: *.cpp
