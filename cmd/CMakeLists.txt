include(CheckIncludeFiles)
set(CMDS
  xcp
  xsh)

foreach(cmd ${CMDS})
  add_executable(${cmd} ${cmd}.cc)
  # to find generated header
  target_include_directories(${cmd} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(${cmd} common xlib libssh2)
endforeach()

check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(sys/select.h HAVE_SYS_SELECT_H)
check_include_files(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(arpa/inet.h HAVE_ARPA_INET_H)
check_include_files(netinet/in.h HAVE_NETINET_IN_H)
check_include_files(winsock2.h HAVE_WINSOCK2_H)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/libssh2_config_cmake.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/libssh2_config.h)

