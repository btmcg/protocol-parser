#￼Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


# GNU Make built-in targets
# ----------------------------------------------------------------------

# delete the target if a recipe fails with a non-zero status
.DELETE_ON_ERROR:

# standardize on good ol' Bourne shell
SHELL := /bin/sh


# output directories
# ----------------------------------------------------------------------
BIN_DIR := bin
LIB_DIR := lib


# settings due to cmd-line-specified targets
# ----------------------------------------------------------------------

# clang-tidy won't work with another compiler's flags/options, so set
# COMPILER to clang if user provided 'tidy' as the target
ifeq (tidy,$(findstring tidy,$(MAKECMDGOALS)))
  COMPILER := clang
endif


# default command variables
# ----------------------------------------------------------------------
AR      := ar
CP      := cp --force
DOXYGEN := doxygen
FORMAT  := clang-format -i --verbose
MKDIR   := mkdir --parents
MV      := mv --force
RM      := rm --force
RMDIR   := rmdir
TIDY    := clang-tidy


# compiler and linker flags
# ----------------------------------------------------------------------
TARGET_ARCH := -march=native

# c/c++ warning flags
WARN := \
  -pedantic-errors \
  -Wall \
  -Wcast-align \
  -Wcast-qual \
  -Wdisabled-optimization \
  -Wempty-body \
  -Werror \
  -Wextra \
  -Wfloat-equal \
  -Wformat=2 \
  -Wmissing-include-dirs \
  -Wno-deprecated-declarations \
  -Wundef \
  -Wuninitialized

# c-specific warning flags
CC_WARN :=

# c++-specific warning flags
CXX_WARN := \
  -fno-operator-names \
  -Wctor-dtor-privacy \
  -Wnon-virtual-dtor \
  -Woverloaded-virtual \
  -Wzero-as-null-pointer-constant

# sanitizer options
ifdef ASAN
  # address sanitizer
  # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
  # https://clang.llvm.org/docs/AddressSanitizer.html
  # https://github.com/google/sanitizers/wiki/AddressSanitizer

  # ASAN_OPTIONS=check_initialization_order=1 bin/test-runner
  CPPFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif

ifdef MSAN
  # memory sanitizer (only supported by clang)
  # https://clang.llvm.org/docs/MemorySanitizer.html

  COMPILER := clang
  CPPFLAGS += -fsanitize=memory -fPIE -pie -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -Wno-unused-command-line-argument
endif

ifdef UBSAN
  # undefined behavior sanitizer
  # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
  # https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html

  # UBSAN_OPTIONS=print_stacktrace=1 bin/test-runner
  CPPFLAGS += -fsanitize=undefined -fno-omit-frame-pointer
endif

# optimization flags
ifdef DEBUG
  OPTFLAGS := -O0 -fno-inline
  WARN += -Wno-error
else
  OPTFLAGS += -O3 -DNDEBUG
endif


# flags for archive-maintaining program
ARFLAGS := rcsv

# compiler flags
CPPFLAGS += -ggdb3 -fstrict-aliasing $(WARN) -MMD $(OPTFLAGS) -iquote src
CXXFLAGS += -std=c++2a $(CXX_WARN)
CFLAGS   += -std=c11 $(CC_WARN)

# linker flags
LDFLAGS += -Wl,-rpath=$(LIB_DIR),--enable-new-dtags
LDLIBS  += -ldl -lrt -pthread


# compiler-specific additions/changes
# ----------------------------------------------------------------------
# either 'gcc' or 'clang'
COMPILER ?= gcc

ifeq (,$(findstring $(COMPILER),gcc clang))
  $(error "Invalid value COMPILER=$(COMPILER), must be either 'gcc' or 'clang'")
endif

include mk/env_$(COMPILER).mk
