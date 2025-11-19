cmake_minimum_required(VERSION 3.22)

project(amethyst)

option(AMY_BUILD_3DS "Build for 3ds" ON)
option(AMY_GOD_DEV "Turn this on if you think you are god" OFF)

add_library(${PROJECT_NAME} STATIC
    source/amethyst.cpp
    source/image.cpp
    source/renderer.cpp
    source/texture.cpp
    source/utils.cpp
    source/c3d.cpp
    source/ctru.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
if(${AMY_BUILD_3DS})
    target_link_libraries(${PROJECT_NAME} PUBLIC m z ctru citro3d)
    target_compile_definitions(${PROJECT_NAME} PUBLIC AMY_3DS)
endif()

add_subdirectory(example)

install(TARGETS ${PROJECT_NAME})
