#include "RANSAC.h"

#include <opencv2/opencv.hpp>


class PlaneExtractor {

    realsense::RANSAC ransac;
public:
    void run(cv::Mat &depth_image);
};