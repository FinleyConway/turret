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

# add cmake vendors
add_subdirectory(vendors/json) 

# main
add_executable(turret
    src/main.cpp
    src/config/application_config.cpp
    src/core/gpio.cpp
    src/core/application.cpp
)

target_link_libraries(turret PUBLIC
    nlohmann_json::nlohmann_json
)

target_include_directories(turret PUBLIC
    ${CMAKE_SOURCE_DIR}/vendors/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/src # provide explicit cleaner headers
)

# try to find WiringPi
find_path(WIRINGPI_INCLUDE_DIR wiringPi.h)
find_library(WIRINGPI_LIBRARY wiringPi)

if(WIRINGPI_INCLUDE_DIR AND WIRINGPI_LIBRARY)
    message(STATUS "WiringPi found, enabling GPIO support")

    target_include_directories(turret PRIVATE
        ${WIRINGPI_INCLUDE_DIR}
    )

    target_link_libraries(turret PRIVATE
        ${WIRINGPI_LIBRARY}
    )

    target_compile_definitions(turret PRIVATE
        HAS_WIRINGPI
    )
else()
    message(STATUS "WiringPi not found, GPIO support disabled")
endif()