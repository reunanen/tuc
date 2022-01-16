cmake_minimum_required(VERSION 3.13)
project(tuc)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
  include
  )

file (GLOB test
  "tests/*.cpp"
  )

add_executable(tuc-tests ${test})

target_compile_options(tuc-tests PRIVATE -Wall -Wextra -Wpedantic -Werror ${OpenMP_CXX_FLAGS})
target_link_libraries(tuc-tests PRIVATE stdc++fs pthread ${OpenMP_CXX_FLAGS})
