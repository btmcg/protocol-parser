# clang-specific options
# ----------------------------------------------------------------------

CC  := clang
CXX := clang++


# warnings
# ----------------------------------------------------------------------
# c/c++ warning flags
WARN +=

# c-specific warning flags
CC_WARN +=

# c++-specific warning flags
CXX_WARN += \
  -Wsuggest-final-methods \
  -Wsuggest-final-types \

CXXFLAGS += -stdlib=libc++

# use the clang linker
LDFLAGS += -rtlib=compiler-rt -fuse-ld=lld
LDLIBS += -lgcc_s
