project(antpie)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)


INCLUDE_DIRECTORIES(
    include/ir
    include/asm
    include/cgen
    include/opt
    include/util
    include
)

add_subdirectory(src/ir)
add_subdirectory(src/asm)
add_subdirectory(src/cgen)
add_subdirectory(src/opt)
add_subdirectory(src/util)

add_executable(
        compiler
        src/main.cc
)

target_link_libraries(
        compiler
        ir
        asm
        cgen
        opt
        util
)

install(
        TARGETS compiler
        RUNTIME DESTINATION bin
)