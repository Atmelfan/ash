cmake_minimum_required(VERSION 3.6)
project(linalg_test)

set(CMAKE_C_STANDARD 99)


set(SOURCE_FILES main.c linalg/linalg.c testext.c testext.h linalg/linalg_utils.h)
add_executable(linalg_test ${SOURCE_FILES})
target_link_libraries(linalg_test m)