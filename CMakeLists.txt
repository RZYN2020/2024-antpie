project(antpie)

find_library(ANTLR4_LIBRARY
  NAMES antlr4-runtime antlr-runtime
  PATHS /usr/local/lib
  REQUIRED)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)


INCLUDE_DIRECTORIES(
    include/ir
    include/asm
    include/cgen
    include/opt
    include/util
    include
    parser
    /usr/local/include/antlr4-runtime
)

add_subdirectory(src/ir)
add_subdirectory(src/asm)
add_subdirectory(src/cgen)
add_subdirectory(src/opt)
add_subdirectory(src/util)
add_subdirectory(parser)

add_executable(
        compiler
        src/main.cc
)

target_link_libraries(
        compiler PRIVATE
        asm
        cgen
        opt
        ir 
        opt
        util
        parser
        ${ANTLR4_LIBRARY}
)

install(
        TARGETS compiler
        RUNTIME DESTINATION bin
)