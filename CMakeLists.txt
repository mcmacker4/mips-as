cmake_minimum_required(VERSION 3.19)
project(asm C)
set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

add_executable(asm src/main.c src/assembler.c src/assembler.h src/buffer.c src/buffer.h src/parse/parser.c src/parse/parser.h
        src/parse/tokenizer.c src/parse/tokenizer.h include/mips-as/prelude.h)

target_include_directories(asm PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_directories(asm PUBLIC ${GLIB_LIBRARY_DIRS})
target_include_directories(asm PUBLIC ${GLIB_INCLUDE_DIRS})
target_link_libraries(asm ${GLIB_LIBRARIES})
