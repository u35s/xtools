aux_source_directory(xlib XLIB_SRCS)
MESSAGE(STATUS "xlib:" +${XLIB_SRCS})
add_library(xlib ${XLIB_SRCS})

aux_source_directory(common COMMON_SRCS)
MESSAGE(STATUS "common:" +${COMMON_SRCS})
add_library(common ${COMMON_SRCS})
target_link_libraries(common xlib)
