cmake_minimum_required(VERSION 3.14.5)
project(calculator_demo LANGUAGES C VERSION 1.0)

add_executable(calc main.c)

set(link_libm ON)

if(link_libm)
target_link_libraries(calc m)
endif()