cmake_minimum_required (VERSION 3.14)

# Define the project
project (testcmsisdsp VERSION 0.1)

option(HOST "" ON)
option(DISABLEFLOAT16 "" ON)
add_subdirectory(${CMSISDSP}/Source bin_dsp)

find_package(OpenCV)


set(NODESHEADER ${CMAKE_SOURCE_DIR}/../../cmsiscv_nodes/cpp)

set(CVLIBSRC ${CMAKE_SOURCE_DIR}/../../Source)

add_executable(stream)


target_sources(stream PUBLIC
    stream/common.c
    )

add_subdirectory(${CVLIBSRC} bin_cv)
add_subdirectory(stream/tests/canny_edge)
add_subdirectory(stream/tests/gaussian)

target_include_directories(stream PUBLIC ${CVLIBHEADER} ${NODESHEADER} stream .)


#target_link_libraries(stream PUBLIC CMSISDSP)
target_link_libraries(stream PUBLIC ${OpenCV_LIBRARIES})
target_link_libraries(stream PUBLIC CMSISCV)


target_compile_options(stream PUBLIC -g -O3 -sUSE_PTHREADS=0)
target_compile_definitions(stream PUBLIC FORCE_SCALAR WEB)

#target_link_options(stream PUBLIC 
#    -sALLOW_MEMORY_GROWTH=0 
#    -sWASM=1 
#    -sASSERTIONS=2
#    -sSTACK_OVERFLOW_CHECK=1
#    -sMALLOC=emmalloc-memvalidate-verbose
#    -sABORTING_MALLOC=1
#    -sSAFE_HEAP=1
#    -sEXPORTED_FUNCTIONS=_init_cv,_stream,_malloc 
#    -sEXPORTED_RUNTIME_METHODS=ccall,cwrap)

# WASM=0 compile to javascript and not wasm 
# it is slower but does not require a server to run the demo

include(exported.cmake)

string(REPLACE ";" "," DEST "${EXPORTED}")

#message(WARNING "${DEST}")

target_link_options(stream PUBLIC 
    -sALLOW_MEMORY_GROWTH=0 
    -sWASM=0
    -sNO_DISABLE_EXCEPTION_CATCHING
    -sEXPORTED_FUNCTIONS=${DEST}
    -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
    -sUSE_PTHREADS=0)

add_custom_command(
        TARGET stream  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/stream.js
                ${CMAKE_CURRENT_SOURCE_DIR}/web/stream.js)