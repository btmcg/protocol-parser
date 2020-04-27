#include "common/compiler.hpp"
#include "parser/parser.hpp"
#include <filesystem>
#include <getopt.h>
#include <unistd.h>
#include <cstdio>
#include <string>


namespace { // unnamed

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

    try {
        parser parser(args.input_file);
        parser.parse();
        parser.print_stats();
    } catch (std::exception const& e) {
        std::fprintf(stderr, "exception caught: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
