add_definitions(-std=c++11 -Os)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED COMPONENTS system filesystem)

set(src MediaCloudLoginProvider.cpp MediaCloudPermissionProvider.cpp)
set(boostlibs ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
set(libs ${boostlibs} sqlite3 avutil avcodec avformat)

add_library(security ${src})
target_link_libraries(security ${libs} mcloud)
