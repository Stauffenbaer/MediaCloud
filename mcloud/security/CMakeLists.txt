add_definitions(-std=c++11)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 REQUIRED COMPONENTS system filesystem)

set(src MediaCloudLoginProvider.cpp MediaCloudPermissionProvider.cpp)
set(boostlibs ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
set(libs ${boostlibs} ao sqlite3 avutil avcodec avformat)

add_library(security ${src})
target_link_libraries(security ${libs})