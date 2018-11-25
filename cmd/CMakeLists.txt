include(CheckIncludeFiles)
set(CMDS
  ssh2
  ssh2_exec
  xcp
  xsh)

foreach(cmd ${CMDS})
  add_executable(${cmd} ${cmd}.cc)
  # to find generated header
  target_include_directories(${cmd} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(${cmd} common xlib libssh2)
endforeach()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/libssh2_config_cmake.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/libssh2_config.h)

