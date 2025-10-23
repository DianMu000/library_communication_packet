
aux_source_directory(. src)

add_executable(code_generator ${src})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
