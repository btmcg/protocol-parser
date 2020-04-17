#include "protocol/soup_bin_tcp.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <getopt.h>
#include <gnu/libc-version.h>
#include <unistd.h>
#include <cstdio>
#include <string>


namespace { // unnamed

    std::string
    get_compiler_version()
    {
#if (defined(__GNUC__) && !defined(__clang__))
        return fmt::format("gcc-{}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
        return fmt::format(
                "clang-{}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
#endif
    }

    std::string
    get_version_info_multiline()
    {
        return fmt::format("compiler_version={}\n"
                           "glibc_version={}",
                get_compiler_version(), ::gnu_get_libc_version());
    }

    struct Args
    {
        std::string input_file;
    };

    Args
    arg_parse(int argc, char** argv)
    {
        auto usage = [](FILE* outerr, std::filesystem::path const& app) {
            std::fprintf(outerr,
                    "usage: %s [-hv] <input_file>\n"
                    "arguments:\n"
                    "   input_file              input file\n"
                    "options:\n"
                    "  -h, --help               this output\n"
                    "  -v, --version            version\n",
                    app.string().c_str());
            std::exit(outerr == stdout ? EXIT_SUCCESS : EXIT_FAILURE);
        };

        auto const app = std::filesystem::path(argv[0]).filename();
        if (argc == 1)
            usage(stderr, app);

        Args args;
        while (true) {
            static option long_options[] = {
                    {"help", no_argument, nullptr, 'h'},
                    {"version", no_argument, nullptr, 'v'},
                    {nullptr, 0, nullptr, 0},
            };

            int const c = ::getopt_long(argc, argv, "hv", long_options, nullptr);
            if (c == -1)
                break;

            switch (c) {
                case 'h':
                    usage(stdout, app);
                    break;

                case 'v':
                    std::fprintf(stdout, "%s\n", get_version_info_multiline().c_str());
                    std::exit(EXIT_SUCCESS);
                    break;

                case '?':
                default:
                    usage(stderr, app);
                    break;
            }
        } // while

        if (optind == argc) {
            std::fprintf(stderr, "missing required argument(s)\n\n");
            usage(stderr, app);
        }

        std::string input_file;
        for (; optind != argc; ++optind) {
            if (args.input_file.empty()) {
                args.input_file = argv[optind];
            } else {
                std::fprintf(stderr, "extra argument(s): %s\n\n", argv[optind]);
                usage(stderr, app);
            }
        }
        return args;
    }

} // namespace


int
main(int argc, char** argv)
{
    Args const args = arg_parse(argc, argv);
    return 0;
}
