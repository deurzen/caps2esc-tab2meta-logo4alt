cmake_minimum_required(VERSION 3.0)

project(caps2esclogo4alt)

add_executable(caps2esclogo4alt caps2esclogo4alt.c)
target_compile_options(caps2esclogo4alt PRIVATE -Wall -Wextra)

install(TARGETS caps2esclogo4alt RUNTIME DESTINATION bin)
