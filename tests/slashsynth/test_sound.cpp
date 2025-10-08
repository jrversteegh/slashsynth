#include "../test_common.h"

#include <cmath>

#include <slashsynth/sound.h>

using namespace slashsynth;

BOOST_AUTO_TEST_CASE(sample_construction_test) {
  Sample sample1{0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
  Sample sample2({0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
  Samples samples;
  samples.push_back(sample1);
  samples.push_back(sample2);
  samples.push_back(Sample({0.f, 0.f, 0.f, 0.f, 0.f, 0.f}));
  BOOST_TEST(samples.size() == 3);
}

BOOST_AUTO_TEST_CASE(sample_addition_test) {
  Sample sample1{1.f, 0.f, 0.f, 0.f, 0.f, 0.f};
  Sample sample2{1.f, 0.f, 0.f, 0.f, 0.f, 0.f};
  auto sample = sample1 + sample2;
  BOOST_TEST(sample.channel_values[0] == std::sqrtf(2));
}

BOOST_AUTO_TEST_CASE(sample_multiplication_test) {
  Sample sample{1.f, 2.f, 0.f, 0.f, 0.f, 0.f};
  sample *= 0.5;
  BOOST_TEST(sample[1] == 1.f);
}

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
