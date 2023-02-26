include(ExternalProject)

ExternalProject_Add(re2c
    PREFIX lexer_re2c
    URL https://github.com/skvadrik/re2c/releases/download/3.0/re2c-3.0.tar.xz
    URL_HASH SHA1=04b4d8fd0a643e270a00de0f545d3ef51213b771
    CMAKE_ARGS -DRE2C_BUILD_RE2GO=OFF -DRE2C_BUILD_RE2RUST=OFF
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(re2c BINARY_DIR)

set(RE2C_EXECUTABLE ${BINARY_DIR}/re2c PARENT_SCOPE)

macro(add_re2c_target INPUT OUTPUT)

    get_filename_component(FULL_INPUT ${INPUT} REALPATH)
    get_filename_component(FULL_OUTPUT ${OUTPUT} ABSOLUTE)

    add_custom_command(OUTPUT ${FULL_OUTPUT}
        COMMAND ${RE2C_EXECUTABLE} -8 -i --no-generation-date -W ${FULL_INPUT} -o ${FULL_OUTPUT}
        DEPENDS re2c ${FULL_INPUT}
        COMMENT "Generating ${FULL_OUTPUT} using ${FULL_INPUT}"
    )

endmacro()
