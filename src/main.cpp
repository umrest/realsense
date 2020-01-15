#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/features2d.hpp>

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

using namespace std;
using namespace cv;

#include "ContourDetector.h"

template <typename Pixel>
struct Map
{
    double in_min, in_max, out_min, out_max;

    Map(double _in_min, double _in_max, double _out_min, double _out_max) : in_min(_in_min), in_max(_in_max), out_min(_out_min), out_max(_out_max)
    {
    }

    void operator()(Pixel &pixel, const int *position) const
    {
        pixel = (pixel - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};

int main()
{
    ContourDetector contour_detector;

    // Create a Pipeline - this serves as a top-level API for streaming and processing frames
    rs2::pipeline p;

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

        Mat depth_image(Size(w, h), CV_16UC1, (void *)depth.get_data(), Mat::AUTO_STEP);

        Mat depth_8bit;
        depth_image.convertTo(depth_8bit, CV_8UC1, 1 / 16.0);

        // Generate mask to fill in holes

        Mat mask1;
        Mat mask2;

        cv::threshold(depth_8bit, mask1, 10, 255, cv::THRESH_BINARY);
        cv::threshold(depth_8bit, mask2, 245, 255, cv::THRESH_BINARY_INV);

        Mat mask = ~(mask1 & mask2);

        cv::inpaint(depth_8bit, mask, depth_8bit, 2, cv::INPAINT_TELEA);

        // Generate mask to ignore edge of image

        cv::Mat mask_edges = cv::Mat::zeros(depth_8bit.size(), depth_8bit.type());

        int padding = 100;

        mask_edges(cv::Rect(padding, padding, mask_edges.cols - 2 * padding, mask_edges.rows - 2 * padding)) = 255;

        Mat contour;
        contour_detector.run(depth_8bit, contour);

        Mat c;
        contour.copyTo(c, mask_edges);

        cv::imshow("depth", depth_8bit);
        cv::imshow("contour", c);

        cv::waitKey(100);

        cout << i++ << endl;
    }
}