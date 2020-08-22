#pragma once

#include <fmt/chrono.h>
#include <cstdint>
#include <ctime>
#include <string>


// free functions
constexpr inline std::timespec ts_diff(std::timespec const& t1, std::timespec const& t2);
constexpr inline std::timespec to_timespec(std::uint64_t nsecs);
constexpr inline std::uint64_t to_nsecs(std::timespec const&);
std::string to_utc_str(std::uint64_t nsecs);
std::string to_time_str(std::uint64_t nsecs);


/// FIXME
class tsc
{
private:
    static inline double ticks_per_nsec_
            = 0.0; ///< initialized by init(), should match cpu clock speed
    static inline std::uint64_t init_ticks_ = 0; ///< ticks recorded at rdtscp init()

public:
    enum : std::uint64_t
    {
        NsecInUsec = 1'000,
        NsecInMsec = 1'000'000,
        NsecInSec = 1'000'000'000,
        InitTime = 1'000'000,
    };

public:
    /// initialize rdtsc. should be called from a cpu-pinned process
    static inline void
    init()
    {
        std::timespec begin_ts = {0, 0};
        std::timespec end_ts = {0, 0};

        ::clock_gettime(CLOCK_REALTIME, &begin_ts);
        auto const begin = rdtscp();

        for (std::size_t i = 0; i < InitTime; ++i)
            __asm__ __volatile__(""); // don't optimize out this loop

        auto const end = rdtscp();
        ::clock_gettime(CLOCK_REALTIME, &end_ts);

        std::timespec const diff = ts_diff(begin_ts, end_ts);
        auto const elapsed = (diff.tv_sec * tsc::NsecInSec) + diff.tv_nsec;
        ticks_per_nsec_ = static_cast<double>(end - begin) / static_cast<double>(elapsed);
        init_ticks_ = begin;
    }

    /// return nanoseconds since epoch
    static std::uint64_t
    gettime_nsec()
    {
        return (rdtscp() - init_ticks_) / static_cast<std::uint64_t>(ticks_per_nsec_);
    }

    /// return current time (nsec since epoch) in the form of a timespec
    static std::timespec
    gettime_ts()
    {
        std::uint64_t const now_nsec = gettime_nsec();

        std::timespec const ts = {static_cast<std::time_t>(now_nsec / tsc::NsecInSec),
                static_cast<long>(now_nsec % tsc::NsecInSec)};
        return ts;
    }

    /// return number of ticks per nsec for this cpu
    static double
    get_ticks_per_nsec()
    {
        return ticks_per_nsec_;
    }

    /// return nanoseconds since system start
    static std::uint64_t
    get_nsecs()
    {
        return rdtscp() / ticks_per_nsec_;
    }

private:
    /// return number of ticks since system start
    static std::uint64_t
    rdtscp()
    {
        std::uint32_t lo_ticks = 0;
        std::uint32_t hi_ticks = 0;

        __asm__ __volatile__("rdtscp"
                             : "=a"(lo_ticks), "=d"(hi_ticks) // output operands
                             : "a"(0)                         // input operands
                             : "%ebx", "%ecx"                 // clobbered registers
        );

        static constexpr std::uint32_t bits = std::numeric_limits<std::uint32_t>::digits;
        return ((static_cast<std::uint64_t>(hi_ticks) << bits) | lo_ticks);
    }

}; // class tsc


/**********************************************************************/
// free functions

constexpr std::timespec
ts_diff(std::timespec const& t1, std::timespec const& t2)
{
    std::uint64_t const n1 = to_nsecs(t1);
    std::uint64_t const n2 = to_nsecs(t2);
    if (n1 > n2)
        return std::timespec{0, 0};

    return to_timespec(n2 - n1);
}

constexpr std::timespec
to_timespec(std::uint64_t nsecs)
{
    std::timespec const ts = {static_cast<std::time_t>(nsecs / tsc::NsecInSec),
            static_cast<long>(nsecs % tsc::NsecInSec)};
    return ts;
}

constexpr std::uint64_t
to_nsecs(std::timespec const& ts)
{
    return ts.tv_nsec + (ts.tv_sec * tsc::NsecInSec);
}
