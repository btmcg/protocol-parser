#include "assert.hpp"
#include <fmt/format.h>
#include <cstdlib>


void
handle_failed_debug_assertion(
        char const* msg, char const* func, char const* file, int line) noexcept
{
    fmt::print(stderr, "debug assertion failure in {} ({}:{}): {}\n", func, file, line, msg);
    std::abort();
}
