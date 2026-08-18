cmake_minimum_required(VERSION 3.16)

project(pimpl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(file_watcher STATIC
    file_watcher.cpp
)

target_include_directories(file_watcher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(file_watcher PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(pimpl
    main.cpp
)

target_link_libraries(pimpl PRIVATE
    file_watcher
)
