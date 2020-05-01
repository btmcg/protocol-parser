#pragma once

#include <fmt/chrono.h>
#include <cstdint>
#include <ctime>
#include <string>


// constants
constexpr std::uint64_t NanosInSec = 1'000'000'000;

// free functions
constexpr inline timespec ts_diff(timespec const& t1, timespec const& t2);
constexpr inline timespec to_timespec(std::uint64_t nsecs);
constexpr inline std::uint64_t to_nsecs(timespec const&);
inline std::string to_utc_str(std::uint64_t nsecs);
inline std::string to_time_str(std::uint64_t nsecs);


/// FIXME
class tsc
{
private:
    static double ticks_per_nsec_; ///< initialized by init(), should match cpu clock speed
    static std::uint64_t last_nsec_; ///< current nsecs since epoch at last calculation
    static std::uint64_t init_ticks_; ///< ticks recorded at rdtscp init()

public:
    static constexpr std::uint64_t NsecInUsec = 1'000;
    static constexpr std::uint64_t NsecInMsec = 1'000'000;
    static constexpr std::uint64_t NsecInSec  = 1'000'000'000;

public:
    /// initialize rdtsc. should be called from a cpu-pinned process
    static inline void init();

    /// return nanoseconds since epoch
    static inline std::uint64_t gettime_nsec();

    /// return current time (nsec since epoch) in the form of a timespec
    static inline timespec gettime_ts();

    /// return number of ticks per nsec for this cpu
    static inline double get_ticks_per_nsec();

    /// return nanoseconds since system start
    static inline std::uint64_t get_nsecs();

private:
    /// return number of ticks since system start
    static inline std::uint64_t rdtscp();

}; // class tsc

/**********************************************************************/

void
tsc::init()
{
    timespec begin_ts = {0, 0};
    timespec end_ts = {0, 0};

    ::clock_gettime(CLOCK_REALTIME, &begin_ts);
    auto const begin = rdtscp();

    for (auto i = 0; i < 1000000; ++i)
        __asm__ __volatile__(""); // don't optimize out this loop

    auto const end = rdtscp();
    ::clock_gettime(CLOCK_REALTIME, &end_ts);

    timespec const diff = ts_diff(begin_ts, end_ts);
    auto const elapsed = (diff.tv_sec * NanosInSec) + diff.tv_nsec;
    ticks_per_nsec_ = static_cast<double>(end - begin) / static_cast<double>(elapsed);
    init_ticks_ = begin;
    last_nsec_ = (begin_ts.tv_sec * NanosInSec) + begin_ts.tv_nsec;
}

std::uint64_t
tsc::gettime_nsec()
{
    std::uint64_t const ticks = rdtscp();
    last_nsec_ += (ticks - init_ticks_) / ticks_per_nsec_;
    return last_nsec_;
}

timespec
tsc::gettime_ts()
{
    std::uint64_t const now_nsec = gettime_nsec();

    timespec const ts = {
        static_cast<std::time_t>(now_nsec / NsecInSec),
        static_cast<long>(now_nsec % NsecInSec)
    };
    return ts;
}

/// return number of ticks per nsec for this cpu
double
tsc::get_ticks_per_nsec()
{
    return ticks_per_nsec_;
}

/// return nanoseconds since system start
std::uint64_t
tsc::get_nsecs()
{
    return rdtscp() / ticks_per_nsec_;
}

std::uint64_t
tsc::rdtscp()
{
    std::uint32_t lo_ticks = 0;
    std::uint32_t hi_ticks = 0;

    __asm__ __volatile__("rdtscp"
                         : "=a"(lo_ticks), "=d"(hi_ticks) // output operands
                         : "a"(0) // input operands
                         : "%ebx", "%ecx" // clobbered registers
    );

    return ((static_cast<std::uint64_t>(hi_ticks) << 32) | lo_ticks);
}

/**********************************************************************/
// free functions

constexpr inline timespec
ts_diff(timespec const& t1, timespec const& t2)
{
    std::uint64_t n1 = to_nsecs(t1);
    std::uint64_t n2 = to_nsecs(t2);
    if (n1 > n2)
        return timespec{0, 0};

    return to_timespec(n2 - n1);
}

constexpr inline timespec
to_timespec(std::uint64_t nsecs)
{
    timespec ts = {static_cast<std::time_t>(nsecs / NanosInSec),
            static_cast<std::int64_t>(nsecs % NanosInSec)};
    return ts;
}

constexpr inline std::uint64_t
to_nsecs(timespec const& ts)
{
    return ts.tv_nsec + (ts.tv_sec * NanosInSec);
}

inline std::string
to_utc_str(std::uint64_t nsecs)
{
    std::time_t const t = nsecs /= NanosInSec;
    return fmt::format("{:%Y-%m-%d %H:%M:%S}.{:09}", *std::gmtime(&t), nsecs);
}

inline std::string
to_time_str(std::uint64_t nsecs)
{
    std::uint64_t s = nsecs / NanosInSec;
    nsecs %= NanosInSec;
    std::uint64_t m = s / 60;
    s %= 60;
    std::uint64_t h = m / 60;
    m %= 60;
    return fmt::format("{:02}:{:02}:{:02}.{:09}", h, m, s, nsecs);
}