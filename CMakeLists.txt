cmake_minimum_required(VERSION 2.6)
project(mediacloud)

add_executable(mediacloud main.cpp)

install(TARGETS mediacloud RUNTIME DESTINATION bin)
