include_directories("../include/")


ENABLE_TESTING()

find_package( OpenCV REQUIRED )
find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
find_package (Threads REQUIRED)
find_package(realsense2 REQUIRED)

link_directories( /usr/local/lib/comm )

add_executable (test_ransac test_ransac.cpp)

target_link_libraries (test_ransac
                       librealsense ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${OpenCV_LIBS}
                      ${realsense2_LIBRARY} 
                      comm
                       )

add_test(NAME test_basic COMMAND test_ransac)
