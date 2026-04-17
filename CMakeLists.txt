cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(diffusion)

set(SOURCE_LIB 
    functions/BackgroundGas.cpp
    functions/LightGas.cpp
    functions/Particle.cpp
)

add_library(diffusion_functions STATIC ${SOURCE_LIB})

find_package(nlohmann_json 3.2.0 REQUIRED)

target_include_directories(diffusion_functions 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/functions)

target_link_libraries(diffusion_functions 
    PRIVATE nlohmann_json::nlohmann_json
)

add_executable(main main.cpp)

target_link_libraries(main 
    PRIVATE diffusion_functions
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config.json 
    ${CMAKE_CURRENT_BINARY_DIR}/config/config.json COPYONLY
)