cmake_minimum_required(VERSION 3.16)
project(piglatin CXX)

set(CMAKE_CXX_STANDARD 17)

#
# Import our library - ideally this should be in a FindPigLatinConverter.cmake
#
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(PigLatinConverter_LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/libPigLatinConverter/lib/linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(PigLatinConverter_LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/libPigLatinConverter/lib/macos)
elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(PigLatinConverter_LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/libPigLatinConverter/lib/windows)
endif()
find_library(PigLatinConverter_LIB
        NAMES PigLatinConverter
        PATHS ${PigLatinConverter_LIBDIR}
        NO_DEFAULT_PATH
        )
if(NOT PigLatinConverter_LIB)
    message(FATAL_ERROR "Failed to find PigLatinConverter library for ${CMAKE_SYSTEM_NAME}")
else()
    message(STATUS "PigLatinConverter found at: ${PigLatinConverter_LIB}")
endif()
add_library(PigLatinConverter UNKNOWN IMPORTED)
set_target_properties(PigLatinConverter PROPERTIES
        IMPORTED_LOCATION ${PigLatinConverter_LIB}
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/deps/libPigLatinConverter
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
        )

#
# Other Dependencies
#
find_package(Threads REQUIRED)

#
# Our executable
#
add_executable(piglatin "")
target_link_libraries(piglatin PRIVATE
        Threads::Threads
        PigLatinConverter
        )
target_sources(piglatin PRIVATE
        async_converter.cc
        async_converter.h
        main.cc
        )
