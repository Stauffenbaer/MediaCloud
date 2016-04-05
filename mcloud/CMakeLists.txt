add_definitions(-std=c++11)

set(mcloudSRC MediaCloudFilesystem.cpp MediaCloudDecoder.cpp MediaCloudDatabase.cpp MediaCloudServer.cpp MediaCloudClient.cpp MediaCloudDecoder.cpp)
set(mcloudLIBS ${Boost_LIBRARIES} sqlite3 avutil avcodec avformat)

link_libraries(${mcloudLIBS})
add_library(mcloud ${mcloudSRC})