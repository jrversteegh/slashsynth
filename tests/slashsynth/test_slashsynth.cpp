#include "../test_common.h"

BOOST_AUTO_TEST_CASE(slashsynth_test) {
  BOOST_TEST(true);
}

struct slashsynthFixture {};

BOOST_FIXTURE_TEST_CASE(slashsynth_fixture_test, slashsynthFixture) {
  BOOST_TEST(true);
};

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
