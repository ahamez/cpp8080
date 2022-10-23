project(cpp8080)
cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

enable_testing()

include_directories("${PROJECT_SOURCE_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/space_invaders")

add_executable(
  space_invaders
  space_invaders/main.cc
  space_invaders/sdl.cc
  space_invaders/space_invaders.cc)
target_include_directories(space_invaders PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(space_invaders SDL2::SDL2)

include_directories("${PROJECT_SOURCE_DIR}/cpu_test")
add_executable(
  cpu_test
  cpu_test/main.cc
  cpu_test/md5.cc)

add_test(
  NAME cpu_test
  COMMAND cpu_test
    60 # timeout (s)
    "${PROJECT_SOURCE_DIR}/cpu_test/roms/8080EXM.COM"
    "${PROJECT_SOURCE_DIR}/cpu_test/roms/8080PRE.COM"
    "${PROJECT_SOURCE_DIR}/cpu_test/roms/CPUTEST.COM"
    "${PROJECT_SOURCE_DIR}/cpu_test/roms/TST8080.COM"
    )
