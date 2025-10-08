cmake_minimum_required(VERSION 3.28)

set(CM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${CM_PATH})

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif()
message(NOTICE "Building: ${CMAKE_BUILD_TYPE}")

include(SetCompiler)

project(slashsynth)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
  set(TEST_LINK_FLAGS "")
  set(MODULE_OUTPUT_DIR ".")
else()
  set(TEST_LINK_FLAGS LINK_FLAGS "-Wl,--no-as-needed")
  set(MODULE_OUTPUT_DIR "${CMAKE_BUILD_TYPE}")
endif()

find_package(Boost 1.88.0 REQUIRED
  log log_setup unit_test_framework regex program_options
)
find_package(fmt 11.2.0 REQUIRED)
find_package(SoundIo 2.0.1 REQUIRED)

if(BUILD_PYTHON)
  find_package(pybind11 REQUIRED)
  execute_process(COMMAND python-config --includes OUTPUT_VARIABLE Python_INCLUDES)
endif()
if(BUILD_BENCHMARKS)
  find_package(benchmark REQUIRED)
endif()

execute_process(COMMAND git show --no-patch --format=%h OUTPUT_VARIABLE GITREV)
execute_process(COMMAND git show --no-patch --format=%ci --date=short OUTPUT_VARIABLE GITDATE)
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/slashsynth")
set(BINDING_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/pycxxslashsynth")
set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/slashsynth")
set(BENCHMARKS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/benchmarks")
set(VERSION_H_IN ${INCLUDE_DIR}/slashsynth/version.h.in)
set(VERSION_H ${INCLUDE_DIR}/slashsynth/version.h)
set(CONFIG_H_IN ${INCLUDE_DIR}/slashsynth/config.h.in)
set(CONFIG_H ${INCLUDE_DIR}/slashsynth/config.h)
configure_file(${VERSION_H_IN} ${VERSION_H} @ONLY)
configure_file(${CONFIG_H_IN} ${CONFIG_H} @ONLY)
if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
  configure_file(${CMAKE_SOURCE_DIR}/cmake/CMakeSettings.json.in ${CMAKE_SOURCE_DIR}/CMakeSettings.json @ONLY)
endif()

include_directories(
  ${INCLUDE_DIR}
)
add_compile_definitions(
  FMT_HEADER_ONLY=1
)

FILE(GLOB lib_sources ${SOURCE_DIR}/*.cpp)
FILE(GLOB binding_sources ${BINDING_SOURCE_DIR}/*.cpp)

add_library(slashsynthlib OBJECT ${lib_sources})
message("Module output directory: ${MODULE_OUTPUT_DIR}")
set_target_properties(slashsynthlib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_OUTPUT_DIR})
target_link_libraries(slashsynthlib PRIVATE
  fmt::fmt
  SoundIo::SoundIo
)
if(BUILD_SHARED)
  set_property(TARGET slashsynthlib PROPERTY POSITION_INDEPENDENT_CODE ON)
  add_library(slashsynth_shared SHARED $<TARGET_OBJECTS:slashsynthlib>)
  set_target_properties(slashsynth_shared PROPERTIES OUTPUT_NAME slashsynth)
endif()
add_library(slashsynth_static STATIC $<TARGET_OBJECTS:slashsynthlib>)
set_target_properties(slashsynth_static PROPERTIES OUTPUT_NAME slashsynth)

add_executable(slashsynth ${lib_sources} src/slashsynth/app/main.cpp)
target_link_libraries(slashsynth PRIVATE
  fmt::fmt
  Boost::program_options
  Boost::log
  SoundIo::SoundIo
)

if(BUILD_PYTHON)
  pybind11_add_module(pycxxslashsynth MODULE ${binding_sources})
  target_link_libraries(pycxxslashsynth PRIVATE
    fmt::fmt
    slashsynth_static
  )
endif()

enable_testing()

if(BUILD_TESTS)
  if(CMAKE_slashsynth_NAME STREQUAL slashsynth_NAME)
      include(CTest)
  endif()

  macro(new_test NEW_TEST)
    set(TEST_SOURCES ${ARGN})
    list(TRANSFORM TEST_SOURCES PREPEND ${SOURCE_DIR}/ )
    add_executable(${NEW_TEST}
      ${TESTS_DIR}/${NEW_TEST}.cpp
      ${TEST_SOURCES}
    )
    target_include_directories(${NEW_TEST} PRIVATE
      ${INCLUDE_DIR}
      ${SOURCE_DIR}
    )
    set_target_properties(${NEW_TEST} PROPERTIES
      ${TEST_LINK_FLAGS}
      RUNTIME_OUTPUT_DIRECTORY tests
    )
    target_link_libraries(${NEW_TEST} PRIVATE
      Boost::unit_test_framework
      fmt::fmt
      SoundIo::SoundIo
      slashsynth_static
    )
    add_test(${NEW_TEST} tests/${NEW_TEST})
  endmacro()

  new_test(test_clock)
  new_test(test_sound)
  new_test(test_output)
  new_test(test_slashsynth)
endif()

if(BUILD_BENCHMARKS)
  if (NOT CMAKE_BUILD_TYPE MATCHES "Release")
    message(WARNING "Expected Release build when building benchmarks")
  endif()

  macro(new_benchmark NEW_BENCHMARK)
    set(BENCHMARK_SOURCES ${ARGN})
    list(TRANSFORM BENCHMARK_SOURCES PREPEND ${SOURCE_DIR}/ )
    add_executable(${NEW_BENCHMARK}
      ${BENCHMARKS_DIR}/${NEW_BENCHMARK}.cpp
      ${BENCHMARK_SOURCES}
    )
    target_include_directories(${NEW_BENCHMARK} PRIVATE
      ${INCLUDE_DIR}
      ${SOURCE_DIR}
    )
    set_target_properties(${NEW_BENCHMARK} PROPERTIES
      ${BENCHMARK_LINK_FLAGS}
      RUNTIME_OUTPUT_DIRECTORY benchmarks
    )
    target_link_libraries(${NEW_BENCHMARK} PRIVATE
      benchmark::benchmark_main
      fmt::fmt
      slashsynth_static
    )
    add_test(${NEW_BENCHMARK} benchmarks/${NEW_BENCHMARK})
  endmacro()

  new_benchmark(benchmark_slashsynth)
endif()
