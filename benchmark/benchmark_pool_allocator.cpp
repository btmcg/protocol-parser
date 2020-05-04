#include "pool_allocator/pool_allocator.hpp"
#include <benchmark/benchmark.h>
#include <list>
#include <random>

class object
{
public:
    object(int a, int b, int c)
            : a_(a)
            , b_(b)
            , c_(c)
    {}
    int
    get_a() const
    {
        return a_;
    }
    int
    get_b() const
    {
        return b_;
    }
    int
    get_c() const
    {
        return c_;
    }

private:
    int a_;
    int b_;
    int c_;
};

static void
pa_list_emplace(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10000);

    std::list<object, pool_allocator<object, 100'000'000>> list;
    for (auto _ : state) {
        list.emplace_back(dist(rng), dist(rng), dist(rng));
    }
}
BENCHMARK(pa_list_emplace);

static void
std_list_emplace(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10000);

    std::list<object> list;
    for (auto _ : state) {
        list.emplace_back(dist(rng), dist(rng), dist(rng));
    }
}
BENCHMARK(std_list_emplace);

static void
pa_list_push(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10000);

    std::list<object, pool_allocator<object, 100'000'000>> list;
    for (auto _ : state) {
        list.push_back(object(dist(rng), dist(rng), dist(rng)));
    }
}
BENCHMARK(pa_list_push);

static void
std_list_push(benchmark::State& state)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10000);

    std::list<object> list;
    for (auto _ : state) {
        list.push_back(object(dist(rng), dist(rng), dist(rng)));
    }
}
BENCHMARK(std_list_push);
