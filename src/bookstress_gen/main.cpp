#include "parser.hpp"
#include "version.hpp"
#include "file_reader/file_reader.hpp"
#include "util/compiler.hpp"
#include <filesystem>
#include <getopt.h>
#include <cstdio> // std::fprintf
#include <cstdlib> // std::exit
#include <string>


namespace { // unnamed

    struct cli_args
    {
        std::filesystem::path input;
        std::filesystem::path output;
    };

    cli_args
    arg_parse(int argc, char** argv)
    {
        auto usage = [](std::FILE* outerr, std::filesystem::path const& app) {
            std::fprintf(outerr,
                    "usage: %s [-hv] <input_file> <output_file>\n"
                    "arguments:\n"
                    "   input_file              input file\n"
                    "   output_file             output file (binary)\n"
                    "options:\n"
                    "  -h, --help               this output\n"
                    "  -v, --version            version\n",
                    app.c_str());
            std::exit(outerr == stdout ? EXIT_SUCCESS : EXIT_FAILURE);
        };

        auto const app = std::filesystem::path(argv[0]).filename();
        if (argc == 1)
            usage(stderr, app);

        cli_args args;
        while (true) {
            static option long_options[] = {
                    {"help", no_argument, nullptr, 'h'},
                    {"version", no_argument, nullptr, 'v'},
                    {nullptr, 0, nullptr, 0},
            };

            int const c = ::getopt_long(
                    argc, argv, "hv", static_cast<option const*>(long_options), nullptr);
            if (c == -1)
                break;

            switch (c) {
                case 'h':
                    usage(stdout, app);
                    break;

                case 'v':
                    std::fprintf(stdout, "app_version=%s\n%s\n", ::VERSION,
                            get_version_info_multiline().c_str());
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
            if (args.input.empty()) {
                args.input = argv[optind];
            } else if (args.output.empty()) {
                args.output = argv[optind];
            } else {
                std::fprintf(stderr, "extra argument(s): %s\n\n", argv[optind]);
                usage(stderr, app);
            }
        }

        if (args.input.empty() || args.output.empty()) {
            std::fprintf(stderr, "missing required argument(s)\n\n");
            usage(stderr, app);
        }

        return args;
    }

} // namespace


int
main(int argc, char** argv)
{
    cli_args const args = arg_parse(argc, argv);

    try {
        file_reader reader(args.input);

        book_stress::parser parser(args.output);
        reader.process_file([&parser](auto ptr, auto len) { return parser.parse(ptr, len); });
    } catch (std::exception const& e) {
        std::fprintf(stderr, "exception caught: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
