
#include "ContourDetector.h"

#include <iostream>

struct ContourComparatorSize
{
    bool operator()(Contour& lhs, Contour& rhs){
        return lhs.radius < rhs.radius;
    }
};

std::vector<Contour> ContourDetector::run(cv::Mat &img)
    {
        std::vector<std::vector<cv::Point>> _contours;
        std::vector<cv::Vec4i> hierarchy;

        std::vector<Contour> contours;

        findContours(img, _contours, hierarchy, cv::RetrievalModes::RETR_TREE, cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        for (size_t i = 0; i < _contours.size(); ++i) {
            float radius = 0;
            cv::Point2f center;
            cv::minEnclosingCircle(_contours[i], center, radius);

            float area = cv::contourArea(_contours[i]);

            float ratio = area / ((2*radius)*(2*radius));

            if(radius > 10 && ratio > .2){
                 Contour new_contour;
                new_contour.x = center.x;
                new_contour.y = center.y;
                new_contour.radius = radius;
                contours.push_back(new_contour);

            }

           
            
        }

        std::sort(contours.begin(), contours.end(), ContourComparatorSize());

        return contours;
    }