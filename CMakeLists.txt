cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(diffusion)

set(SOURCE_LIB 
    src/BackgroundGas.cpp
    src/LightGas.cpp
    src/Particle.cpp
)

add_library(diffusion_functions STATIC ${SOURCE_LIB})

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(VTK REQUIRED COMPONENTS CommonCore CommonDataModel IOXML)
find_package(OpenMP REQUIRED)

target_include_directories(diffusion_functions 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

target_link_libraries(diffusion_functions 
    PRIVATE 
        nlohmann_json::nlohmann_json
        VTK::CommonCore
        VTK::CommonDataModel
        VTK::IOXML
        OpenMP::OpenMP_CXX 
)

add_executable(main src/main.cpp)

target_Link_libraries(main
    PRIVATE diffusion_functions
)

target_compile_options(main PRIVATE 
    -O3
    -march=native
    -ffast-math
    -flto
)

if(VTK_VERSION VERSION_GREATER_EQUAL "9.2.0")
    vtk_module_autoinit(
        TARGETS main diffusion_functions
        MODULES VTK::CommonCore VTK::CommonDataModel VTK::IOXML
    )
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config.json 
    ${CMAKE_CURRENT_BINARY_DIR}/config/config.json COPYONLY
)