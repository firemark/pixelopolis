find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

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