cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(hbftms
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# Find dependencies
# TODO: fetch dependencies if not found
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SFML REQUIRED window)
find_package(SFML REQUIRED system)

include(FetchContent)

# Try to find GLM, fetch if not found
find_package(GLM QUIET)

if (NOT GLM_FOUND)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY  https://github.com/g-truc/glm
        GIT_TAG         0.9.9.8
        PREFIX          "build"
        INSTALL_COMMAND ""
    )

    # Including the downloaded library
    FetchContent_GetProperties(glm)
    if(NOT glm_POPULATED)
      FetchContent_Populate(glm)
      include_directories(${glm_SOURCE_DIR}/)
    endif()
endif()

# Set the executable output path to bin/
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

function(create_resource dir output)

    # Collect input files
    file(GLOB shaders ${dir}/*)

    # Remove output if any input files have been modified
    foreach(shader ${shaders})
        if(${shader} IS_NEWER_THAN ${output})
            file(REMOVE ${output})
        endif(${shader} IS_NEWER_THAN ${output})
    endforeach()

    # Write output if it doesn't exist
    if(NOT EXISTS ${output})
        file(WRITE ${output} "#pragma once\n")
        foreach(shader ${shaders})

            # Get short filename
            string(REGEX MATCH "([^/]+)$" filename ${shader})

            # Clean spaces and extension separator
            string(REGEX REPLACE "\\.| |-" "_" filename ${filename})

            # Read source from file
            file(READ ${shader} filedata)

            # Append data to output file as a C++ raw string literal
            file(APPEND ${output} "const char ${filename}[] = R\"(${filedata})\";\n")

        endforeach()
    endif(NOT EXISTS ${output})
endfunction()

create_resource(shaders ${PROJECT_SOURCE_DIR}/src/shaders.hpp)

# -I flags for the compiler
include_directories(src)

add_subdirectory(src)
