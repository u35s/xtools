cmake_minimum_required(VERSION 2.6)

project(px)

add_definitions(-std=c++11)

include_directories(src)

aux_source_directory(src/xlib XLIB_SRCS)
MESSAGE(STATUS "src/xlib:" +${XLIB_SRCS})

aux_source_directory(src/common COMMON_SRCS)
MESSAGE(STATUS "src/common:" +${COMMON_SRCS})

aux_source_directory(src/cmd/xcp XCP_SRCS)
MESSAGE(STATUS "xcp:" +${XCP_SRCS})
add_executable( xcp ${XCP_SRCS} ${COMMON_SRCS} ${XLIB_SRCS})

aux_source_directory(src/cmd/xsh XSH_SRCS)
MESSAGE(STATUS "xsh:" +${XSH_SRCS})
add_executable( xsh ${XSH_SRCS} ${COMMON_SRCS} ${XLIB_SRCS})


file(COPY "conf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

