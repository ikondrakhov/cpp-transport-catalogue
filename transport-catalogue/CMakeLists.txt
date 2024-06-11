cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(transport-catalogue)

set(CMAKE_CXX_STANDARD 17)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(
        CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} /JMC"
    )
else()
    set(
        CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-implicit-fallthrough"
    )
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

file(GLOB sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(
    transport-catalogue
    ${sources}
)