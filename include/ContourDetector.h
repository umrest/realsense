#include "median.h"

class ContourDetector
{
private:
public:
    Mat canny_output;
    void run(cv::Mat &img, cv::Mat &contour_image)
    {
        double sigma = .33;

        double v = cv::median(img);

        int min = int(std::max(0.0, (1.0 - sigma) * v));
        int max = int(std::min(255.0, (1.0 + sigma) * v));

        cv::Canny(img, canny_output, min, max);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        contour_image = Mat::zeros(canny_output.size(), CV_8UC3);

        for (int i = 0; i < contours.size(); i++)
        {
            Scalar color = Scalar(255, 0, 0);
            drawContours(contour_image, contours, i, color, 2, 8, hierarchy, 0, Point());
        }
    }
};