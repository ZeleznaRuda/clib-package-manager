cmake_minimum_required(VERSION 3.15)
project(cclm VERSION 4.4.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CCLM_DEV_MODE "Enable development helpers" OFF)

add_executable(cclm
    src/main.cpp
    src/toolsf.cpp
    src/argvf.cpp
    src/yamlf.cpp
    src/clif.cpp
    src/transactionf.cpp
    src/appf.cpp
)

target_include_directories(cclm PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(cclm PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

if(CCLM_DEV_MODE)
    add_custom_target(compile_commands_link ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()

install(TARGETS cclm
    RUNTIME DESTINATION bin
)
