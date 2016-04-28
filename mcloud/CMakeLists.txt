add_definitions(-std=c++11 -fpermissive)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem thread)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(mcloudSRC MediaCloudSettings.cpp MediaCloudFilesystem.cpp MediaCloudDecoder.cpp MediaCloudDatabase.cpp MediaCloudServer.cpp MediaCloudClient.cpp MediaCloudDecoder.cpp MediaCloudAudioPlayer.cpp MediaCloudAudioProvider.cpp MediaCloudUtils.cpp)
set(mcloudboost ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
set(mcloudLIBS ${mcloudboost} sqlite3 avutil avformat)

add_library(mcloud ${mcloudSRC})
target_link_libraries(mcloud security)
target_link_libraries(mcloud ${mcloudLIBS})

include(${CMAKE_ROOT}/Modules/FindPkgConfig.cmake)

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

set_target_properties(mcloud
                      PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS}
                                 LINKER_FLAGS ${CMAKE_LINKER_FLAGS})

add_subdirectory(security)
