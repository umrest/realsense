cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
project( realsense )

file(GLOB vision_SRC
    "include/*.h"
    "src/*.cpp"
)

include_directories( include )

find_package( OpenCV REQUIRED )

add_executable( realsense ${vision_SRC} )

target_link_libraries( realsense ${OpenCV_LIBS} )