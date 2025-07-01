cmake_minimum_required(VERSION 3.14)
project(serenity VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Platform detection and setup
if(NOT DEFINED PLATFORM)
    set(PLATFORM "Desktop")
endif()

if(MSVC)
    set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/raylib-5.5_win64_msvc16")
else()
    set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/raylib-5.5_linux_amd64")
endif()
set(RAYLIB_INCLUDE_DIR "${RAYLIB_DIR}/include")
set(RAYLIB_LIB_DIR "${RAYLIB_DIR}/lib")


# Collect source files (exclude any backup or temporary files)
file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# Remove any unwanted files
list(FILTER SOURCES EXCLUDE REGEX ".*~$")
list(FILTER SOURCES EXCLUDE REGEX ".*\\.bak$")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${RAYLIB_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include 
)

# Link raylib
target_link_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_LIB_DIR})
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        raylib 
        winmm 
        gdi32 
        opengl32 
        shell32
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE RAYLIB_STATIC)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib m pthread dl GL)
endif()


# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /W4 
        /wd4996
        /std:c++20
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${PROJECT_NAME} PRIVATE /Od /Zi)
        target_link_options(${PROJECT_NAME} PRIVATE /DEBUG)
    endif()
else()
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wpedantic
        -Wno-unused-parameter
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
    endif()
endif()

# Web/Emscripten specific settings
if(${PLATFORM} MATCHES "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PRIVATE
        -sUSE_GLFW=3
        -sWASM=1
        -sEXPORTED_FUNCTIONS=['_main']
        -sEXPORTED_RUNTIME_METHODS=ccall
        -sTOTAL_MEMORY=67108864
        -sALLOW_MEMORY_GROWTH=1
        --preload-file ${CMAKE_SOURCE_DIR}/assets@assets/
    )
    # Set emscripten-specific compiler flags
    target_compile_options(${PROJECT_NAME} PRIVATE -sUSE_GLFW=3)
endif()

# Copy assets to build directory
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
    )
    add_dependencies(${PROJECT_NAME} copy_assets)

    file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")
    foreach(ASSET_FILE ${ASSET_FILES})
        file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/assets ${ASSET_FILE})
        configure_file(${ASSET_FILE} ${CMAKE_BINARY_DIR}/assets/${REL_PATH} COPYONLY)
    endforeach()
endif()

# Visual Studio working directory
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

# Install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION bin)
endif()

# Message
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Using local raylib from ${RAYLIB_DIR}")