#include "../test_common.h"

#include <chrono>
#include <cmath>
#include <thread>

#include <slashsynth/clock.h>

using namespace slashsynth;

BOOST_AUTO_TEST_CASE(clock_resolution_test) {
  using namespace std::chrono;
  using namespace std::chrono_literals;
  auto const half_period = microseconds(10);
  auto start = high_resolution_clock::now();
  auto next = start;

  for (int i = 0; i < 100000; ++i) {
    next += half_period;
    std::this_thread::sleep_until(next);
  }
  auto end = high_resolution_clock::now();
  auto delay = end - start - 1s;

  BOOST_TEST(delay < 1ms);
}

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
