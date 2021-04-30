cmake_minimum_required(VERSION 3.4.3)
project(wak_lang C)


set(CMAKE_C_STANDARD 11)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please use the build script. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()


file(GLOB_RECURSE SOURCE_FILES RELATIVE "${PROJECT_SOURCE_DIR}" src/*.c)
list(REMOVE_ITEM SOURCE_FILES src/main.c)

set(GCC_FLAGS "-march=native -Werror=implicit -Wall -Wextra -Wno-unused-parameter --save-temps -foptimize-sibling-calls")
set(CLANG_FLAGS "")
set(MSVC_FLAGS "/W4")

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_FLAGS}")
endif()
if ( MSVC )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MSVC_FLAGS}")
endif()

add_executable(wak_lang ${SOURCE_FILES} src/main.c)
target_include_directories(wak_lang PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_executable(lex_speed ${SOURCE_FILES} tests/lex_speed.c)
target_include_directories(lex_speed PUBLIC "${PROJECT_SOURCE_DIR}/include")

source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})
