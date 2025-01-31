cmake_minimum_required(VERSION 3.14)
project(serenity VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(DEFINED PLATFORM)
    set(PLATFORM ${PLATFORM})
else()
    set(PLATFORM "Desktop")
endif()

find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(raylib)
endif()

set(SOURCES 
    src/game.cpp
    src/main.cpp
    src/terrain.cpp
    src/perlin.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
    if(NOT WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
    endif()
endif()

if(${PLATFORM} MATCHES "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PRIVATE 
        -sEXPORTED_FUNCTIONS=['_main','_malloc']
        -sEXPORTED_RUNTIME_METHODS=ccall
        -sUSE_GLFW=3
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
endif()