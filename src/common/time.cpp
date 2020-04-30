#include "time.hpp"


double tsc::cycles_per_nsec_ = 0.0;
std::uint64_t tsc::last_nsec_ = 0;
std::uint64_t tsc::init_cycles_ = 0;
