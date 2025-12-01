cmake_minimum_required(VERSION 3.22)

project(amethyst)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(AMY_GOD_DEV "Turn this on if you think you are god" OFF)
# THis option should be disabled if you use STB_IMAGE in you main project
set(AMY_BUILD_STB_IMAGE "Include STB Image code" CACHE BOOL 1)

#add_subdirectory(vendor/libpicasso)

add_library(${PROJECT_NAME} STATIC
    source/app.cpp
    source/assets.cpp
    source/amethyst.cpp
    source/image.cpp
    source/renderer.cpp
    source/texture.cpp
    source/utils.cpp
    source/c3d.cpp
    source/ctru.cpp
    source/iron/iron.cpp
    source/iron/drawlist.cpp
    source/maths/mat.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
#target_link_libraries(${PROJECT_NAME} PUBLIC pica::pica)
target_link_libraries(${PROJECT_NAME} PUBLIC m z ctru citro3d)
target_compile_definitions(${PROJECT_NAME} PUBLIC AMY_3DS AMY_STB_IMAGE=${AMY_BUILD_STB_IMAGE})

add_subdirectory(example)

install(TARGETS ${PROJECT_NAME})
