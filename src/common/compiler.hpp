#pragma once

#include <fmt/format.h>
#include <string>
#include <gnu/libc-version.h>   // ::gnu_get_libc_version


std::string
get_compiler_version()
{
#if (defined(__GNUC__) && !defined(__clang__))
    return fmt::format("gcc-{}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
    return fmt::format("clang-{}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
#endif
}

std::string
get_version_info_multiline()
{
    return fmt::format("compiler_version={}\n"
                       "glibc_version={}",
            get_compiler_version(), ::gnu_get_libc_version());
}
