/**
 * \file
 * \brief Provide python bindings for C++ objects and functions
 *
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 */

#include <pybind11/pybind11.h>

#include "slashsynth/config.h"
#include "slashsynth/version.h"

namespace py = pybind11;

using namespace slashsynth;

PYBIND11_MODULE(pycxxslashsynth, m) {
  m.doc() = "slashsynth C++ module";
  m.attr("__version__") = STRINGIFY(VERSION);
  m.attr("git_revision") = STRINGIFY(GITREV);
  m.attr("git_date") = STRINGIFY(GITDATE);
  m.attr("build_type") = STRINGIFY(BUILD_TYPE);
  m.attr("build_date") = STRINGIFY(BUILD_DATE);
}
