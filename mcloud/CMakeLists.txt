add_definitions(-std=c++11)

set(mcloudSRC MediaCloudDatabase.cpp MediaCloudServer.cpp MediaCloudClient.cpp)
set(mcloudLIBS ${Boost_LIBRARIES} sqlite3)

link_libraries(${mcloudLIBS})
add_library(mcloud ${mcloudSRC})