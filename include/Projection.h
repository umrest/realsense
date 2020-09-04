#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/features2d.hpp>

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <librealsense2/rsutil.h>


using namespace std;
using namespace cv;


class Projection
{
private:
    double angle;                        // in radians
    double scale;
    double height;                       // in meters
    const struct rs2_intrinsics *intrin; // camera parameters
    struct rs2_extrinsics extrin;

public:
    Projection(double _angle, double _height, const struct rs2_intrinsics *_intrin, float _scale)
        : angle(_angle * 3.14159 / 180.0), height(_height), intrin(_intrin), scale(_scale)
    {
        extrin.translation[0] = 0;
        extrin.translation[1] = 1;
        extrin.translation[2] = 0;

        extrin.rotation[0] =1;
        extrin.rotation[1] = 0;
        extrin.rotation[2] = 0;
        extrin.rotation[3] = 0;
        extrin.rotation[4] = cos(-angle);
        extrin.rotation[5] = -sin(-angle);
        extrin.rotation[6] = 0;
        extrin.rotation[7] = sin(-angle);
        extrin.rotation[8] = cos(-angle);
    }

    void project(Mat &src, Mat &dst);
};