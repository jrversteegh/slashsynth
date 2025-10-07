#include "../test_common.h"

#include <slashsynth/sound.h>

using namespace slashsynth;

BOOST_AUTO_TEST_CASE(sample_test) {
  Sample sample1{0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
  Sample sample2({0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
  Samples samples;
  samples.push_back(sample1);
  samples.push_back(sample2);
  samples.push_back(Sample({0.f, 0.f, 0.f, 0.f, 0.f, 0.f}));
  BOOST_TEST(samples.size() == 3);
}

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
