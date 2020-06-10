#include "version.hpp"
#include "file_reader/file_reader.hpp"
#include "itch/parser.hpp"
#include "util/compiler.hpp"
#include "util/time.hpp"
#include <filesystem>
#include <getopt.h>
#include <cstdio> // std::fprintf
#include <cstdlib> // std::exit
#include <string>


namespace { // unnamed

    struct cli_args
    {
        std::string input_file;
        bool logging = false;
        std::filesystem::path stats_fp;
    };

    cli_args
    arg_parse(int argc, char** argv)
    {
        auto usage = [](std::FILE* outerr, std::filesystem::path const& app) {
            std::fprintf(outerr,
                    "usage: %s [-hlv] [-s <stats_file>] <input_file>\n"
                    "arguments:\n"
                    "   input_file              input file\n"
                    "options:\n"
                    "  -h, --help               this output\n"
                    "  -l, --log                log protocol msgs to <protocol>.log\n"
                    "  -s, --stats=<filepath>   record instrument stats to file\n"
                    "  -v, --version            version\n",
                    app.string().c_str());
            std::exit(outerr == stdout ? EXIT_SUCCESS : EXIT_FAILURE);
        };

        auto const app = std::filesystem::path(argv[0]).filename();
        if (argc == 1)
            usage(stderr, app);

        cli_args args;
        while (true) {
            static option long_options[] = {
                    {"help", no_argument, nullptr, 'h'},
                    {"log", no_argument, nullptr, 'l'},
                    {"stats", required_argument, nullptr, 's'},
                    {"version", no_argument, nullptr, 'v'},
                    {nullptr, 0, nullptr, 0},
            };

            int const c = ::getopt_long(
                    argc, argv, "hls:v", static_cast<option const*>(long_options), nullptr);
            if (c == -1)
                break;

            switch (c) {
                case 'h':
                    usage(stdout, app);
                    break;

                case 'l':
                    args.logging = true;
                    break;

                case 's':
                    args.stats_fp = optarg;
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
    cli_args const args = arg_parse(argc, argv);

    tsc::init();

    try {
        file_reader reader(args.input_file);

        if (args.logging) {
            itch::parser<true> parser(args.stats_fp.string());
            reader.process_file([&parser](auto ptr, auto len) { return parser.parse(ptr, len); });
            parser.print_stats();
        } else {
            itch::parser<false> parser(args.stats_fp.string());
            reader.process_file([&parser](auto ptr, auto len) { return parser.parse(ptr, len); });
            parser.print_stats();
        }

        reader.print_stats();
    } catch (std::exception const& e) {
        std::fprintf(stderr, "exception caught: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
