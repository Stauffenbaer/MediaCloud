add_definitions(-std=c++11)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 REQUIRED COMPONENTS system filesystem) 

set(mcloudSRC MediaCloudFilesystem.cpp MediaCloudDecoder.cpp MediaCloudDatabase.cpp MediaCloudServer.cpp MediaCloudClient.cpp MediaCloudDecoder.cpp)
set(mcloudboost ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
set(mcloudLIBS ${mcloudboost} id3 sqlite3 avutil avcodec avformat)

link_libraries(${mcloudLIBS})
add_library(mcloud ${mcloudSRC})