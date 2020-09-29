cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 17)
project( realsense )

add_subdirectory (tests)

file(GLOB LIBREALSENSE_SRC
    "include/*.h"
    "src/*.cpp"
)

find_package( OpenCV REQUIRED )

include_directories( include 
 ${BOOST_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

list(FILTER LIBREALSENSE_SRC EXCLUDE REGEX ".*/src/main.cpp")


find_package( Boost COMPONENTS filesystem system REQUIRED )
find_package(realsense2 REQUIRED)
find_package (Threads REQUIRED)


link_directories( /usr/local/lib/comm )

add_library ( librealsense ${LIBREALSENSE_SRC})

add_executable( realsense "src/main.cpp" )

target_link_libraries( realsense librealsense ${OpenCV_LIBS} comm ${realsense2_LIBRARY}  ${CMAKE_THREAD_LIBS_INIT} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})