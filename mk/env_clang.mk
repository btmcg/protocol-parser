# clang-specific options
# --------------------------------------------------------------------

# command variables
# ----------------------------------------------------------------------
# Note: In order for -flto to work properly, gcc-ar must be used.
AR  := llvm-ar rcs
CC  := clang
CXX := clang++


# warnings
# --------------------------------------------------------------------
# c/c++ warning flags
WARN +=

# c-specific warning flags
CC_WARN +=

# c++-specific warning flags
CXX_WARN +=
CXXFLAGS += -stdlib=libc++

# use the clang linker
LDFLAGS += -rtlib=compiler-rt -fuse-ld=lld
LDLIBS += -lgcc_s
