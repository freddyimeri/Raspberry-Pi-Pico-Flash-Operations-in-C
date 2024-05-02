cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(cap_template C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(cap_template
  main.c
  custom_fgets.c
  flash_ops.c
  flash_ops_helper.c
  cli.c
  test.c
)

pico_enable_stdio_usb(cap_template 1)
pico_enable_stdio_uart(cap_template 0)

pico_add_extra_outputs(cap_template)

target_link_libraries(cap_template pico_stdlib)
