cmake_minimum_required(VERSION 3.22)

project(amethyst)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(AMY_GOD_DEV "Turn this on if you think you are god" OFF)
# THis option should be disabled if you use STB_IMAGE in you main project
set(AMY_BUILD_STB_IMAGE ON CACHE BOOL "Build stb_image" FORCE)
set(AMY_BUILD_STB_TRUETYPE ON CACHE BOOL "Build stb_truetype" FORCE)
option(AMY_BUILD_STB_TRUETYPE ON)
option(AMY_BUILD_EXAMPLE OFF)

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
if(AMY_BUILD_STB_IMAGE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC AMY_STB_IMAGE)
endif()
if(AMY_BUILD_STB_TRUETYPE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC AMY_STB_TT)
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
target_link_options(${PROJECT_NAME} PUBLIC
    "-Wl,--wrap=malloc"
    "-Wl,--wrap=free"
    "-Wl,--wrap=realloc"
    "-Wl,--wrap=calloc"
    "-Wl,--wrap,memalign"
    "-Wl,--wrap,aligned_alloc"
)
endif()

target_compile_options(${PROJECT_NAME} PUBLIC -Wno-psabi)

if(${AMY_BUILD_EXAMPLE})
    add_subdirectory(example)
endif()

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY include DESTINATION .)

find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    message(STATUS "Clang Format found.")
    set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set(COMMENT "Formatting Project Sources")
    if(WIN32)
        add_custom_target(${PROJECT_NAME}-clang-format
            COMMAND powershell.exe -Command "Get-ChildItem '${SOURCES}/*' -Include *.cpp,*.hpp -Recurse | Foreach {&'${CLANG_FORMAT}' -i $_.fullname}"
            COMMENT ${CCOMMENT})
    else()
        add_custom_target(${PROJECT_NAME}-clang-format
            COMMAND find ${SOURCES} -iname *.hpp -o -iname *.cpp | xargs ${CLANG_FORMAT} -i
            COMMENT ${COMMENT}
        )
    endif()
    unset(COMMENT)
endif()
