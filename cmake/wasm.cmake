function(add_wasm)
    cmake_parse_arguments(
        PARSE_ARGV 0 ARGS "" "" "SRC_FILES;HEADER_FILES")
    add_executable(PixelopolisWasm ${ARGS_SRC_FILES} ${ARGS_HEADER_FILES})
    set_target_properties(PixelopolisWasm PROPERTIES
        OUTPUT_NAME pixelopolis.js
    )
    target_compile_definitions(PixelopolisWasm PUBLIC pixelopolis)
    target_include_directories(PixelopolisWasm
        PUBLIC include
        PRIVATE lib_include
        ${PNG_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_compile_options(PixelopolisWasm PUBLIC -sUSE_LIBPNG=1 -DWASM=1)
    target_link_options(PixelopolisWasm PUBLIC
        -sASYNCIFY=1
        -sUSE_LIBPNG=1
        -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
        -sASSERTIONS=1
        -sALLOW_MEMORY_GROWTH=1
    )
    # target_link_libraries(PixelopolisWasm ${PNG_LIBRARY} m)
    # target_compile_options(PixelopolisWasm PUBLIC -Werror -Wall -Wextra -Wshadow -pedantic)
    # target_compile_options(PixelopolisWasm PUBLIC -Werror -fno-omit-frame-pointer)
    # install(
    #     TARGETS PixelopolisWasm
    #     PUBLIC_HEADER
    #     LIBRARY DESTINATION lib
    #     INCLUDES DESTINATION include
    # )
endfunction()