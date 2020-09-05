#include "RANSAC.h"

#include <random>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace realsense;

Point::Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z){

}

float Plane::get_z(float x, float y){
    return (D - A * x - B * y) / C;
}

Plane::Plane(float _A, float _B, float _C, float _D) : A(_A), B(_B), C(_C), D(_D){
}

Plane::Plane(Point& a, Point& b, Point& c){
    float A1 = b.x - a.x;
    float B1 = b.y - a.y;
    float C1 = b.z - a.z;

    float A2 = c.x - a.x;
    float B2 = c.y - a.y;
    float C2 = c.z - a.z;

    A = B1 * C2 - B2 * C1; 
    B = A2 * C1 - A1 * C2; 
    C = A1 * B2 - B1 * A2; 

    D = (- A * a.x - B * a.y - C * a.z); 
}

float Plane::dist(Point& a){
    return fabs(A*a.x+B*a.y+C*a.z+D) / sqrt(A*A+B*B+C*C);
}

struct PairComparatorFirst
{
    bool operator ()(const std::pair<float, Plane> & pair1, const std::pair<float, Plane> & pair2)
    {
        return pair1.first < pair2.first;
    }
};


Plane RANSAC::RANSAC::run(std::vector<Point> &points){
    std::vector<std::pair<float, Plane>> total_errors;

    for(int i = 0; i < num_iters; i++){
        std::vector<Point> sampled_points;
        std::sample(points.begin(), points.end(), std::back_inserter(sampled_points),
                    3, std::mt19937{std::random_device{}()});

        Plane sample_plane(sampled_points[0], sampled_points[1], sampled_points[2]);

        float total_error = 0;
        
        for (auto &pt : points){
            float dist = sample_plane.dist(pt);
            total_error += dist;
        }

        total_errors.push_back(std::pair<float, Plane>(total_error, sample_plane));
        
    }

    const auto min = std::min_element(total_errors.begin(), total_errors.end(), PairComparatorFirst());
    const auto max = std::max_element(total_errors.begin(), total_errors.end(), PairComparatorFirst());

    return min->second;

}