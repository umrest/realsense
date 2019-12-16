#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/features2d.hpp>

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API	

using namespace std;
using namespace cv;

#include "ContourDetector.h"

int main(){
    ContourDetector contour_detector;

	// Create a Pipeline - this serves as a top-level API for streaming and processing frames
    rs2::pipeline p;

    rs2::colorizer color_map;

    // Configure and start the pipeline
    p.start();
    int i = 0;

	while (true)
    {
        // Block program until frames arrive
        rs2::frameset frames = p.wait_for_frames();

        // Try to get a frame of a depth image
        rs2::frame depth = frames.get_depth_frame();

        // Query frame size (width and height)
        int w = depth.as<rs2::video_frame>().get_width();
        int h = depth.as<rs2::video_frame>().get_height();

        Mat depth_image(Size(w, h), CV_16UC1, (void*)depth.get_data(), Mat::AUTO_STEP);

        Mat depth_8bit;
        depth_image.convertTo(depth_8bit, CV_8UC1, 1/16.0);


        Mat contour;
        contour_detector.run(depth_8bit, contour);

        cv::imshow("depth", depth_8bit);
        //cv::imshow("contour", contour);
        cv::waitKey(100);
        cout << i++ << endl;
    }
}	