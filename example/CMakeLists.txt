cmake_minimum_required(VERSION 3.22)

find_program(PICASSO NAMES picasso REQUIRED)

# Function origanally Created in Re-Craft-3DS
function(__amy_make_shader arg1 arg2)
    # These only exist cause i was stupid
    set(__FILE ${arg1})
    set(__NAME ${arg2})

    # Need to build shaders during config stage :(
    execute_process(
        COMMAND ${PICASSO} -o "${CMAKE_CURRENT_SOURCE_DIR}/romfs/shaders/${__NAME}.shbin" "${__FILE}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

project(amethyst-example)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

__amy_make_shader(${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader2d.v.pica shader2d)
__amy_make_shader(${CMAKE_CURRENT_SOURCE_DIR}/shaders/lithium.v.pica lithium)

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
