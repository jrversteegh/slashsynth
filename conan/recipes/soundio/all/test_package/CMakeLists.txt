cmake_minimum_required(VERSION 3.15)
project(test_package)

find_package(SoundIO REQUIRED CONFIG)

add_executable(${PROJECT_NAME} test_package.cpp)
target_link_libraries(${PROJECT_NAME} SoundIO::SoundIO)
