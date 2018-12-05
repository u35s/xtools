cmake_minimum_required(VERSION 2.6)

project(px)

add_definitions(-std=c++11 -Wall)

file(COPY "conf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

include_directories(src)

add_subdirectory(thirdparty/libssh2)
add_subdirectory(src)
add_subdirectory(cmd)
