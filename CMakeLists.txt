cmake_minimum_required(VERSION 3.22...3.30)
project(turret LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # hush clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set build dir
set(BINARY_FOLDER "turret")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/${BINARY_FOLDER}"
)

add_executable(turret
    src/main.cpp
)