cmake_minimum_required(VERSION 3.4.3)
project(wak_lang C)


set(CMAKE_C_STANDARD 11)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please use the build script. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()


file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.c")

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif()
if ( MSVC )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /W4")
endif()

add_executable(wak_lang ${SOURCE_FILES})
target_include_directories(wak_lang PUBLIC "${PROJECT_SOURCE_DIR}/include")

source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})
