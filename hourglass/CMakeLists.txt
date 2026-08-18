cmake_minimum_required(VERSION 3.16)
project(hourglass LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generates build/compile_commands.json, which clangd reads for include paths.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra)

add_library(plugin_host STATIC host/plugin.cpp)

target_include_directories(plugin_host PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/host
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(plugin_host PRIVATE ${CMAKE_DL_LIBS})

add_executable(hourglass main.cpp)
target_link_libraries(hourglass PRIVATE plugin_host)
