cmake_minimum_required(VERSION 2.6)
project(mediacloud)

set(CMAKE_AUTOMOC ON)

execute_process(COMMAND pkg-config --cflags gstreamer-1.0 OUTPUT_VARIABLE CMAKE_C_FLAGS)
string(REPLACE "\n" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

execute_process(COMMAND pkg-config --cflags gstreamer-video-1.0 OUTPUT_VARIABLE CMAKE_C_FLAGS)
string(REPLACE "\n" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

execute_process(COMMAND pkg-config --libs gstreamer-1.0 OUTPUT_VARIABLE CMAKE_LINKER_FLAGS)
string(REPLACE "\n" "" CMAKE_LINKER_FLAGS ${CMAKE_LINKER_FLAGS})

execute_process(COMMAND pkg-config --libs gstreamer-video-1.0 OUTPUT_VARIABLE CMAKE_LINKER_FLAGS)
string(REPLACE "\n" "" CMAKE_LINKER_FLAGS ${CMAKE_LINKER_FLAGS})

add_definitions(-std=c++11 -Os)

find_package(Qt4 REQUIRED COMPONENTS QtGui QtCore)

include_directories(${QT_INCLUDE_DIR})

add_executable(mediacloud_server server_main.cpp)
add_executable(mediacloud_client client_main.cpp)

set_target_properties(mediacloud_server PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS} LINKER_FLAGS ${CMAKE_LINKER_FLAGS})
set_target_properties(mediacloud_client PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS} LINKER_FLAGS ${CMAKE_LINKER_FLAGS})

target_link_libraries(mediacloud_server mcloud security -lgstreamer-1.0 -lgstbase-1.0 -lgstvideo-1.0 -lgobject-2.0 -lglib-2.0)
target_link_libraries(mediacloud_client mcloud security -lgstreamer-1.0 -lgstbase-1.0 -lgstvideo-1.0 -lgobject-2.0 -lglib-2.0)

target_link_libraries(mediacloud_client Qt4::QtGui Qt4::QtCore)
target_link_libraries(mediacloud_server Qt4::QtGui Qt4::QtCore)

install(TARGETS mediacloud_server RUNTIME DESTINATION bin)
install(TARGETS mediacloud_client RUNTIME DESTINATION bin)

add_subdirectory(mcloud)