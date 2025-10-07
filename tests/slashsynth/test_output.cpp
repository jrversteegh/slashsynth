#include "../test_common.h"

#include <slashsynth/output.h>

#include <cmath>
#include <thread>

using namespace slashsynth;
using namespace std::chrono_literals;

float square(float value) {
  return value * value;
}

struct Sample_data {
  Sample_data() {
    for (int i = 0; i < 100000; ++i) {
      float i_frac = i / 100000.f;
      float n_frac = 1.f - i_frac;
      sine.push_back({
          float(std::sin((0.03 * (1 + i_frac) * i)) * 0.4 * (1 + n_frac)),
          float(std::sin((0.03 * (1 + n_frac) * i)) * 0.4 * (1 + i_frac)),
          0.f,
          0.f,
          0.f,
          0.f,
      });
    }
    for (int i = 0; i < 2000; ++i) {
      sine[i] *= i / 2000.f;
      sine[sine.size() - 1 - i] *= i / 2000.f;
    }
  }
  Samples sine;
};

BOOST_FIXTURE_TEST_CASE(sndio_output_test, Sample_data) {
  auto sound_io = SoundIO{};
  auto device = sound_io.get_default_output_device();
  auto name = device.get_name();
  BOOST_TEST(name != "");
  auto output = device.get_output();
  output.play(sine);
  std::this_thread::sleep_for(3s);
}

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
