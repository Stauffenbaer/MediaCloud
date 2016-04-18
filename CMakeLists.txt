cmake_minimum_required(VERSION 2.6)
project(mediacloud)


# Set CMAKE_C_FLAGS variable with info from pkg-util
execute_process(COMMAND pkg-config --cflags gstreamer-1.0
                OUTPUT_VARIABLE CMAKE_C_FLAGS)
string(REPLACE "\n" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")

# Set CMAKE_LINKER_FLAGS variable with info from pkg-util
execute_process(COMMAND pkg-config --libs gstreamer-1.0
                OUTPUT_VARIABLE CMAKE_LINKER_FLAGS)
string(REPLACE "\n" "" CMAKE_LINKER_FLAGS ${CMAKE_LINKER_FLAGS})
message("CMAKE_LINKER_FLAGS: ${CMAKE_LINKER_FLAGS}")

add_definitions(-std=c++11)
add_executable(mediacloud main.cpp)
set_target_properties(mediacloud
                      PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS}
                                 LINKER_FLAGS ${CMAKE_LINKER_FLAGS})

target_link_libraries(mediacloud mcloud security -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0)

install(TARGETS mediacloud RUNTIME DESTINATION bin)

add_subdirectory(mcloud)