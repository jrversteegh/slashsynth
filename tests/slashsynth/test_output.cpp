#include "../test_common.h"

#include <slashsynth/output.h>

using namespace slashsynth;

BOOST_AUTO_TEST_CASE(sndio_output_test) {
  auto sound_io = SoundIO{};
  auto device = sound_io.get_default_output_device();
  auto name = device.get_name();
  BOOST_TEST(name != "");
  auto output = device.get_output();
}

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
