cmake_minimum_required(VERSION 3.22)

project(amethyst-example)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(${PROJECT_NAME} source/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE amethyst)
target_compile_options(${PROJECT_NAME} PRIVATE -O2)

ctr_generate_smdh(
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.smdh
    NAME "Amethyst Example"
    DESCRIPTION "Example of Amethyst lib"
    AUTHOR "tobid7"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/romfs/icon.png"
)

ctr_create_3dsx(
    ${PROJECT_NAME}
    OUTPUT "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.3dsx"
    SMDH "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.smdh"
    ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
)
