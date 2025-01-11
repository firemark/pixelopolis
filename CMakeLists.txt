cmake_minimum_required(VERSION 3.0)
project(Pixelopolis VERSION 0.2.0 LANGUAGES C)

find_package(PNG REQUIRED)

include(cmake/lib.cmake)
include(cmake/exec.cmake)
include(cmake/parser.cmake)
include(cmake/tests.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES src/*.c)
file(GLOB_RECURSE TEST_FILES tests/test_*.c)
file(GLOB_RECURSE HEADER_FILES include/*.h)

compile_parser(PARSER_FILES)
add_lib(SRC_FILES ${SRC_FILES} ${PARSER_FILES} HEADER_FILES ${HEADER_FILES})
add_exec(Pixelopolis pixelopolis)
add_exec(PixelopolisNormalMap pixelopolis-normal-map)
add_tests("${TEST_FILES}")
