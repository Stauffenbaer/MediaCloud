add_definitions(-std=c++11)

set(mcloudSRC MediaCloudDatabase.cpp MediaCloudServer.cpp MediaCloudClient.cpp)
set(mcloudLIBS boost boost-system boost-filesystem sqlite3)

link_libraries(${mcloudLIBS})
add_library(mcloud ${mcloudSRC})