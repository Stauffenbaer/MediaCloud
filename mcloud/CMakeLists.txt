add_definitions(-std=c++11)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 REQUIRED COMPONENTS system filesystem)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(mcloudSRC MediaCloudSettings.cpp MediaCloudFilesystem.cpp MediaCloudDecoder.cpp MediaCloudDatabase.cpp MediaCloudServer.cpp MediaCloudClient.cpp MediaCloudDecoder.cpp)
set(mcloudboost ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
set(mcloudLIBS ${mcloudboost} ao sqlite3 avutil avcodec avformat)

add_library(mcloud ${mcloudSRC})
target_link_libraries(mcloud Qt5::Widgets Qt5::Multimedia)
target_link_libraries(mcloud ${mcloudLIBS})