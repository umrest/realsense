#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

struct Contour
{
    int x;
    int y;
    int radius;
};

class ContourDetector
{
private:
public:
    cv::Mat canny_output;
    std::vector<Contour> run(cv::Mat &img);
};

