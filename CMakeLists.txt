cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
project( realsense )

file(GLOB vision_SRC
    "include/*.h"
    "src/main.cpp"
)

include_directories( include 
 ${BOOST_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )
find_package( Boost COMPONENTS filesystem system REQUIRED )
find_package(realsense2 REQUIRED)
find_package (Threads REQUIRED)


link_directories( /usr/local/lib/comm )

add_executable( realsense ${vision_SRC} )

target_link_libraries( realsense ${OpenCV_LIBS} comm ${realsense2_LIBRARY}  ${CMAKE_THREAD_LIBS_INIT} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})