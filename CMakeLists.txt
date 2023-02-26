cmake_minimum_required(VERSION 3.15)

project(zedit
    VERSION 1.0
    DESCRIPTION "Small crossplatform console line edit library"
    HOMEPAGE_URL https://github.com/masscry/zedit
    LANGUAGES CXX
)

add_subdirectory(3rdparty)

add_library(setup INTERFACE)

target_compile_features(setup
    INTERFACE
        cxx_std_11
)

target_compile_options(setup
    INTERFACE
        -Wall
        -Wextra
        -Werror
        $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
        $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
)

add_re2c_target(src/zdict.re2c ${CMAKE_CURRENT_SOURCE_DIR}/src/zdict.gen.cpp)

add_library(zedit)

target_sources(zedit
    PRIVATE
        src/zedit.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zdict.gen.cpp
        src/ncurses/terminal.cpp
)

target_include_directories(zedit
    PUBLIC
        src/
)

target_link_libraries(zedit
    PUBLIC
        setup
)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

add_executable(zcmd)

target_sources(zcmd
    PRIVATE
        src/zcmd.cpp
)

target_link_libraries(zcmd
    PRIVATE
        zedit
        ${CURSES_LIBRARIES}
)

target_include_directories(zcmd
    PRIVATE
        ${CURSES_INCLUDE_DIRS}
)

target_compile_options(zcmd
    PRIVATE
        ${CURSES_CFLAGS}
)
