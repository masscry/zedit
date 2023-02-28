cmake_minimum_required(VERSION 3.15)

project(zedit
    VERSION 1.0
    DESCRIPTION "Small crossplatform console line edit library"
    HOMEPAGE_URL https://github.com/masscry/zedit
    LANGUAGES CXX
)

if(UNIX)
    add_subdirectory(3rdparty)
endif(UNIX)

add_library(setup INTERFACE)

target_compile_features(setup
    INTERFACE
        cxx_std_11
)

if(UNIX)
target_compile_options(setup
    INTERFACE
        -Wall
        -Wextra
        -Werror
        $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
        $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
)
endif()

if(UNIX)
    add_re2c_target(src/zdict.re2c ${CMAKE_CURRENT_SOURCE_DIR}/src/zdict.gen.cpp)
endif(UNIX)

add_library(zedit)

target_sources(zedit
    PRIVATE
        src/zedit.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zdict.gen.cpp
        $<$<PLATFORM_ID:UNIX>:src/ncurses/terminal.cpp>
        $<$<PLATFORM_ID:WIN32>:src/win32/terminal.cpp>
)

if(UNIX)
target_sources(zedit
    PRIVATE
        src/ncurses/terminal.cpp
)
endif(UNIX)

if(WIN32)
target_sources(zedit
    PRIVATE
        src/win32/terminal.cpp
)
endif(WIN32)

target_include_directories(zedit
    PUBLIC
        src/
)

target_link_libraries(zedit
    PUBLIC
        setup
)

add_library(curses INTERFACE)

if (UNIX)
    set(CURSES_NEED_NCURSES TRUE)
    find_package(Curses REQUIRED)

    target_link_libraries(curses
        INTERFACE
            ${CURSES_LIBRARIES}
            
    )

    target_include_directories(curses
        INTERFACE
            ${CURSES_INCLUDE_DIRS}
    )

    target_compile_options(curses
        INTERFACE
            ${CURSES_CFLAGS}
    )

endif()

add_executable(zcmd)

target_sources(zcmd
    PRIVATE
        src/zcmd.cpp
)

target_link_libraries(zcmd
    PRIVATE
        zedit
        curses
)
