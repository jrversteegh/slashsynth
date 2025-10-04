#include <string>
#include <vector>
#include <array>

#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

#define TEST

namespace tt = boost::test_tools;
namespace ut = boost::unit_test;

#ifdef WIN32
#define LINE_END "\r\n"
#else
#define LINE_END "\n"
#endif
