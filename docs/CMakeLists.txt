cmake_minimum_required(VERSION 3.28)

project(slashsynth_docs)

if (NOT DEFINED VERSION)
  message(FATAL_ERROR "Definition of VERSION is required.")
endif()

find_package(Doxygen)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/slashsynth")

if(DOXYGEN_FOUND)
  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

  add_custom_target(
    doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
  )
else()
  add_custom_target(
    doc ALL
    COMMAND echo "Doxygen not installed. Not building C++ documentation."
    COMMENT "Doxygen not installed"
    VERBATIM
  )
endif()
