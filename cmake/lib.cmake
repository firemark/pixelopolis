function(add_lib)
    cmake_parse_arguments(
        PARSE_ARGV 0 ARGS "" "" "SRC_FILES;HEADER_FILES")
    add_library(PixelopolisLib SHARED ${ARGS_SRC_FILES} ${ARGS_HEADER_FILES})
    set_target_properties(PixelopolisLib PROPERTIES
        OUTPUT_NAME pixelopolis
        VERSION ${PROJECT_VERSION}
    )
    target_compile_definitions(PixelopolisLib PUBLIC HAS_PNG)
    target_include_directories(PixelopolisLib
        PUBLIC include
        PRIVATE lib_include
        ${PNG_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(PixelopolisLib ${PNG_LIBRARY} m)
    target_compile_options(PixelopolisLib PUBLIC -Werror -Wall -Wextra -Wshadow -pedantic)
    # target_compile_options(PixelopolisLib PUBLIC -Werror -fno-omit-frame-pointer)
    install(
        TARGETS PixelopolisLib
        PUBLIC_HEADER
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include
    )
endfunction()