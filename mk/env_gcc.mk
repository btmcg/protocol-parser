# gcc-specific options
# ----------------------------------------------------------------------


# command variables
# ----------------------------------------------------------------------
# Note: In order for -flto to work properly, gcc-ar must be used.
AR  := gcc-ar rcs
CC  := gcc
CXX := g++


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
  -Wsuggest-override \
  -Wuseless-cast
