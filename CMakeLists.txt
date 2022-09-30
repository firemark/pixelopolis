cmake_minimum_required(VERSION 3.0)
project(Pixelopolis VERSION 0.1.5 LANGUAGES C)

find_package(PNG REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

function(main)
    file(GLOB_RECURSE SRC_FILES src/*.c)
    file(GLOB_RECURSE HEADER_FILES include/*.h)
    compile_parser(PARSER_FILES)
    add_lib(SRC_FILES ${SRC_FILES} ${PARSER_FILES} HEADER_FILES ${HEADER_FILES})
    add_exec(Pixelopolis pixelopolis)
    add_exec(PixelopolisNormalMap pixelopolis-normal-map)
endfunction()

function(add_lib)
    cmake_parse_arguments(
        PARSE_ARGV 0 ARGS "" "" "SRC_FILES;HEADER_FILES")
    add_library(PixelopolisLib SHARED ${ARGS_SRC_FILES} ${ARGS_HEADER_FILES})
    set_target_properties(PixelopolisLib PROPERTIES
        OUTPUT_NAME pixelopolis
        VERSION ${PROJECT_VERSION}
    )
    target_compile_definitions(PixelopolisLib PUBLIC pixelopolis)
    target_include_directories(PixelopolisLib
        PUBLIC include
        PRIVATE lib_include
        ${PNG_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(PixelopolisLib
        ${PNG_LIBRARY}
        ${FLEX_LIBRARIES}
        m
    )
    install(
        TARGETS PixelopolisLib
        PUBLIC_HEADER
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include
    )

endfunction()

function(add_exec TARGET NAME)
    add_executable(${TARGET} main/${NAME}.c)
    target_include_directories(${TARGET} PUBLIC include)
    target_link_libraries(${TARGET} PUBLIC PixelopolisLib)
    set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${NAME})
    install(TARGETS ${TARGET} DESTINATION bin)
endfunction()

function(compile_parser SRC_FILES)
    bison_target(PixelopolisParser
        src/css.y
        ${CMAKE_CURRENT_BINARY_DIR}/css_parser.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/css.y.h
    )
    flex_target(PixelopolisScanner
        src/css.l
        ${CMAKE_CURRENT_BINARY_DIR}/css_scanner.c
    )
    add_flex_bison_dependency(PixelopolisScanner PixelopolisParser)
    set(${SRC_FILES} ${BISON_PixelopolisParser_OUTPUTS} ${FLEX_PixelopolisScanner_OUTPUTS} PARENT_SCOPE)
endfunction()

main()
