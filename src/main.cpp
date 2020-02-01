#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/features2d.hpp>

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <librealsense2/rsutil.h>

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

struct Projection
{
private:
    double angle;                        // in radians
    double height;                       // in meters
    const struct rs2_intrinsics *intrin; // camera parameters

public:
    Projection(double _angle, double _height, const struct rs2_intrinsics *_intrin)
        : angle(_angle * 3.14159 / 180.0), height(_height), intrin(_intrin)
    {
    }

    void project(Mat &src, Mat &dst)
    {
        dst = cv::Mat::zeros(cv::Size(20, 20), CV_16UC1);

        for (int y = 0; y < src.rows; y += 1)
        {
            // Loop over all columns
            for (int x = 0; x < src.cols; x += 1)
            {
                uint8_t depth = src.at<uint8_t>(y, x);

                int _x = depth * (x - intrin->ppx) / intrin->fx;
                int _y = depth * (y - intrin->ppy) / intrin->fy;

                if (abs(_x) <= 20 && abs(_y) <= 20)
                {
                    dst.at<uint8_t>(_y, _x) = depth;
                }
            }
        }
    }
};

int main()
{
    ContourDetector contour_detector;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;

    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH); // Enable default depth

    auto profile = pipe.start(cfg);
    //auto intrin = profile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();
    //Our goal is to generate depth without any holes, since these are going to pose an immediate problem to our algorithm. The best way to reduce the number of missing pixels is by letting the hardware do it. The D400 cameras have a High Density preset we can take advantage of.

    // Set the device to Default preset
    auto sensor = profile.get_device().first<rs2::depth_sensor>();
    sensor.set_option(RS2_OPTION_VISUAL_PRESET, RS2_RS400_VISUAL_PRESET_DEFAULT);

    int i = 0;

    while (true)
    {
        std::cout << "Getting frames" << std::endl;
        // Block program until frames arrive
        rs2::frameset frames = pipe.wait_for_frames();

        // Try to get a frame of a depth image
        rs2::frame depth = frames.get_depth_frame();

        // Query frame size (width and height)
        int w = depth.as<rs2::video_frame>().get_width();
        int h = depth.as<rs2::video_frame>().get_height();

        std::cout << "Creating depth map" << std::endl;

        // get depth map and convert it to 8bit
        Mat depth_image(Size(w, h), CV_16UC1, (void *)depth.get_data(), Mat::AUTO_STEP);

        Mat depth_8bit;
        depth_image.convertTo(depth_8bit, CV_8UC1, 1 / 16.0);

        std::cout << "Creating mask" << std::endl;

        // Generate mask to fill in holes

        Mat mask1;
        Mat mask2;

        cv::threshold(depth_8bit, mask1, 10, 255, cv::THRESH_BINARY);
        cv::threshold(depth_8bit, mask2, 245, 255, cv::THRESH_BINARY_INV);

        Mat mask = ~(mask1 & mask2);

        std::cout << "Filling in holes" << std::endl;
        // fill in holes
        cv::inpaint(depth_8bit, mask, depth_8bit, 1, cv::INPAINT_TELEA);
        cv::imshow("depth", depth_8bit);

        std::cout << "Translating pixel" << std::endl;
        // translate pixels
        /*
        Mat dst;
        Projection p(30, .5, &intrin);
        p.project(depth_8bit, dst);

        std::cout << "Showingdst" << std::endl;

        cv::imshow("dd", dst);
*/
        // Generate mask to ignore edge of image
        /*
        Mat contour;
        contour_detector.run(depth_8bit, contour);

        cv::imshow("canny", contour_detector.canny_output);

        cv::imshow("depth", depth_8bit);
        cv::imshow("contour", contour);
        */

        cv::waitKey(1000);

        cout << i++ << endl;
    }
}