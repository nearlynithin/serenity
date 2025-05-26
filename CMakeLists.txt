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

# Find or fetch raylib
find_package(raylib QUIET)
if(NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif()

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
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /W4 
        /wd4996  # Disable deprecated function warnings
    )
    # For better debugging in Visual Studio
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${PROJECT_NAME} PRIVATE /Od /Zi)
        target_link_options(${PROJECT_NAME} PRIVATE /DEBUG)
    endif()
else()
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter  # Common in raylib callbacks
    )
    # Additional flags for debug builds
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
    endif()
endif()

# Platform-specific linking
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
    if(NOT WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
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
    # Create a custom target to copy assets
    add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
    )
    add_dependencies(${PROJECT_NAME} copy_assets)
    
    # For better incremental builds, only copy if assets have changed
    file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")
    foreach(ASSET_FILE ${ASSET_FILES})
        file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR}/assets ${ASSET_FILE})
        configure_file(${ASSET_FILE} ${CMAKE_BINARY_DIR}/assets/${REL_PATH} COPYONLY)
    endforeach()
endif()

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(raylib_FOUND)
    message(STATUS "Using system raylib")
else()
    message(STATUS "Using fetched raylib")
endif()

# Optional: Set working directory for Visual Studio
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

# Optional: Create install target
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
)
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets 
        DESTINATION bin
    )
endif()