#pragma once


#ifdef BTMCG_DEBUG
#    define DEBUG_ASSERT(expr) \
        do { \
            static_cast<void>((expr) \
                    || (handle_failed_debug_assertion( \
                                "\"" #expr "\"", __func__, __FILE__, __LINE__), \
                            true)); \
        } while (0)
#else
#    define DEBUG_ASSERT(expr)
#endif

void handle_failed_debug_assertion(
        char const* msg, char const* func, char const* file, int line) noexcept;
