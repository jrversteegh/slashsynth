cmake_minimum_required(VERSION 3.28)

project(ConanConfig)

set(CM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
list(APPEND CMAKE_MODULE_PATH ${CM_PATH})

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif()
message(NOTICE "Building: ${CMAKE_BUILD_TYPE}")

include(SetCompiler)

FILE(GLOB PROFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/*.profile.in)
set(PROFILE_OUT ${PROFILE_IN})
list(TRANSFORM PROFILE_OUT REPLACE "[.]profile.in$" ".profile")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(COMPILER_FAMILY "gcc")
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (CMAKE_SYSTEM_NAME MATCHED "Darwin")
    set(COMPILER_FAMILY "apple-clang")
  else()
    set(COMPILER_FAMILY "clang")
  endif()
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(COMPILER_FAMILY "msvc")
endif()

string(REPLACE "." ";" COMPILER_VERSION_DIGITS ${CMAKE_CXX_COMPILER_VERSION})
list(GET COMPILER_VERSION_DIGITS 0 COMPILER_MAJOR_VERSION)

message(NOTICE "Configuring ${PROFILE_IN} to ${PROFILE_OUT}")
configure_file(${PROFILE_IN} ${PROFILE_OUT} @ONLY)



