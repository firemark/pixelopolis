cmake_minimum_required(VERSION 3.0)
project(Pixelopolis C)

find_package(PNG REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.c)

set(
    LIBS
    ${PNG_LIBRARY}
    ${FLEX_LIBRARIES}
    m
)
set(
    INCLUDE_DIRS
    include
    ${PNG_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

bison_target(
    PixelopolisParser
    src/css.y
    ${CMAKE_CURRENT_BINARY_DIR}/css_parser.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/css.y.h
)
flex_target(
    PixelopolisScanner
    src/css.l
    ${CMAKE_CURRENT_BINARY_DIR}/css_scanner.c
)
add_flex_bison_dependency(PixelopolisScanner PixelopolisParser)

add_executable(
    Pixelopolis
    ${SRC_FILES}
    ${BISON_PixelopolisParser_OUTPUTS}
    ${FLEX_PixelopolisScanner_OUTPUTS}
)
target_include_directories(Pixelopolis PUBLIC ${INCLUDE_DIRS})
target_link_libraries(Pixelopolis ${LIBS})
set_target_properties(Pixelopolis PROPERTIES OUTPUT_NAME pixelopolis)

install(TARGETS Pixelopolis DESTINATION bin)
