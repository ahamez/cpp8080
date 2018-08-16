project(cpp8080)
cmake_minimum_required(VERSION 3.12)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/space_invaders")

add_executable(
  space_invaders
  space_invaders/main.cc
  space_invaders/sdl.cc)
target_link_libraries(space_invaders ${CONAN_LIBS})
