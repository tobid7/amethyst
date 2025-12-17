cmake_minimum_required(VERSION 3.22)

project(amethyst)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(AMY_GOD_DEV "Turn this on if you think you are god" OFF)
# THis option should be disabled if you use STB_IMAGE in you main project
set(AMY_BUILD_STB_IMAGE CACHE BOOL 0)
set(AMY_BUILD_STB_TRUETYPE 1)
set(AMY_WITH_MPG123 "Include MP3 Support" CACHE BOOL 1)

add_subdirectory(vendor/libpicasso)

add_library(${PROJECT_NAME} STATIC
    source/app.cpp
    source/assets.cpp
    source/amethyst.cpp
    source/color.cpp
    source/godtrace.cpp
    source/image.cpp
    source/renderer.cpp
    source/texture.cpp
    source/utils.cpp
    source/c3d.cpp
    source/iron/iron.cpp
    source/iron/drawlist.cpp
    source/iron/font.cpp
    source/maths/mat.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DEVKITPRO}/portlibs/3ds/include
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/internal
)
target_link_libraries(${PROJECT_NAME} PUBLIC pica::pica)
target_link_libraries(${PROJECT_NAME} PUBLIC m z ctru citro3d mpg123)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    AMY_3DS
    AMY_STB_IMAGE=${AMY_BUILD_STB_IMAGE}
    AMY_STB_TT=${AMY_BUILD_STB_TRUETYPE}
    AMY_WITH_MPG123=${AMY_WITH_MPG123}
)
target_compile_options(${PROJECT_NAME} PUBLIC -Wno-psabi)

add_subdirectory(example)

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY include DESTINATION .)
