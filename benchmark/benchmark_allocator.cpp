#include "allocator/memory_pool.hpp"
#include "allocator/mp_allocator.hpp"
#include <benchmark/benchmark.h>
#include <algorithm> // std::uniform_int_distribution
#include <cstddef> // std::size_t
#include <cstdint>
#include <functional> // std::equal_to
#include <limits>
#include <list>
#include <random>
#include <unordered_map>
#include <utility>

class object
{
public:
    object(int a, std::uint64_t b, int c) noexcept
            : a_(a)
            , b_(b)
            , c_(c)
    {}
    int
    get_a() const noexcept
    {
        return a_;
    }
    int
    get_b() const noexcept
    {
        return b_;
    }
    int
    get_c() const noexcept
    {
        return c_;
    }

private:
    int a_;
    std::uint64_t b_;
    int c_;
};

static void
mp_list(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(
            0, std::numeric_limits<int>::max());

    constexpr std::size_t node_size = 16 + sizeof(object);
    memory_pool pool(node_size, 100'000'000);
    std::list<object, mp_allocator<object>> list(pool);
    for (auto _ : state) // NOLINT
        list.emplace_back(dist(rng), dist(rng), dist(rng));
}
BENCHMARK(mp_list);

static void
std_list(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(
            0, std::numeric_limits<int>::max());

    std::list<object> list;
    for (auto _ : state) // NOLINT
        list.emplace_back(dist(rng), dist(rng), dist(rng));
}
BENCHMARK(std_list);

static void
mp_unordered_map(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(
            0, std::numeric_limits<int>::max());

    constexpr std::size_t node_size = 16 + sizeof(std::pair<const int, object>);
    memory_pool pool(node_size, 100'000'000);
    std::unordered_map<int, object, std::hash<int>, std::equal_to<int>,
            mp_allocator<std::pair<const int, object>>>
            map(pool);

    for (auto _ : state) // NOLINT
        map.emplace(dist(rng), object(dist(rng), static_cast<std::uint64_t>(dist(rng)), dist(rng)));
}
BENCHMARK(mp_unordered_map);

static void
std_unordered_map(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(
            0, std::numeric_limits<int>::max());

    std::unordered_map<int, object> map;
    for (auto _ : state) // NOLINT
        map.emplace(dist(rng), object(dist(rng), static_cast<std::uint64_t>(dist(rng)), dist(rng)));
}
BENCHMARK(std_unordered_map);
