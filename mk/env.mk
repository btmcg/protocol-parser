# GNU Make built-in targets
# ------------------------------------------------------------------------

# delete the target if a recipe fails with a non-zero status
.DELETE_ON_ERROR:

# standardize on good ol' Bourne shell
SHELL := /bin/sh


# output directories
# ------------------------------------------------------------------------
BIN_DIR := bin
LIB_DIR := lib


# command variables
# ------------------------------------------------------------------------
AR      := ar rcs
CP      := cp --force
DOXYGEN := doxygen
MKDIR   := mkdir --parents
MV      := mv --force
RM      := rm --force
RMDIR   := rmdir


# compiler and linker flags
# ------------------------------------------------------------------------
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
  # ASAN_OPTIONS=check_initialization_order=1 bin/test-runner
  CPPFLAGS = -fsanitize=address -fno-omit-frame-pointer
endif

ifdef MSAN
  CPPFLAGS = -fsanitize=memory -fPIE -pie -fsanitize-memory-track-origins -fno-omit-frame-pointer -Wno-unused-command-line-argument
endif

ifdef UBSAN
  # UBSAN_OPTIONS=print_stacktrace=1 bin/test-runner
  CPPFLAGS = -fsanitize=undefined -fno-omit-frame-pointer
endif

# gcc optimization flags
ifdef DEBUG
  OPTFLAGS := -O0 -fno-inline
  WARN += -Wno-error
else
  OPTFLAGS := -O3 -DNDEBUG
  # OPTFLAGS := -O3 -DNDEBUG -fprofile-generate=pgo
  # OPTFLAGS := -O3 -DNDEBUG -fprofile-use=pgo -Wno-missing-profile
endif

# compiler flags
CPPFLAGS += -ggdb3 -fstrict-aliasing $(WARN) -MMD $(OPTFLAGS) -iquote src
CXXFLAGS += -std=c++2a $(CXX_WARN)
CFLAGS   += -std=c11 $(CC_WARN)

# linker flags
LDFLAGS += -Wl,-rpath=$(LIB_DIR),--enable-new-dtags
LDLIBS  += -ldl -lrt -pthread


# compiler-specific additions/changes
# ------------------------------------------------------------------------
# either 'gcc' or 'clang'
COMPILER := gcc

ifeq (,$(findstring $(COMPILER),gcc clang))
  $(error "Invalid value COMPILER=$(COMPILER), must be either 'gcc' or 'clang'")
endif

include mk/env_$(COMPILER).mk
