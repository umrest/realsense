#include "PlaneExtractor.h"

#include <iostream>

using namespace realsense;

void PlaneExtractor::run(cv::Mat &depth_image){
    std::vector<Point> points;

    for(int y = 0; y < depth_image.rows; ++y) {
        uchar* p = depth_image.ptr(y);
        for(int x = 0; x < depth_image.cols; ++x) {

            int z = *p;
            if (z != 0){
                Point pt(x, y, z);
    
                points.push_back(pt);
            }

            p++;
        }
    }

    Plane best_fit_plane = ransac.run(points);

    for(int y = 0; y < depth_image.rows; ++y) {
        uchar* p = depth_image.ptr(y);
        for(int x = 0; x < depth_image.cols; ++x) {

            int z = *p;
            if (z != 0){
                Point pt(x, y, z);
    
                if(best_fit_plane.dist(pt) < 5){
                    *p = 0;
                }
            }

            p++;
        }
    }
}