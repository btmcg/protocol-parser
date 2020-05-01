#include "time.hpp"


double tsc::ticks_per_nsec_ = 0.0;
std::uint64_t tsc::last_nsec_ = 0;
std::uint64_t tsc::init_ticks_ = 0;
