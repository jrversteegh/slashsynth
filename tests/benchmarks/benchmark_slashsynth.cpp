#include <cassert>
#include <iostream>
#include <random>

#include <benchmark/benchmark.h>

#include "slashsynth/config.h"

using namespace slashsynth;

auto get_random() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<Number> dis(0.0, 1.0);
  return dis(gen);
}

static void benchmark_slashsynth(benchmark::State& state) {
  for (auto _ : state) {
    auto value = get_random();
    benchmark::DoNotOptimize(value);
  }
}
BENCHMARK(benchmark_slashsynth);

BENCHMARK_MAIN();
