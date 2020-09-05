#include <vector>

namespace realsense {
    class Point {
        public:
        float x;
        float y;
        float z;

        Point(float _x, float _y, float _z);
    };

    /* Represented by Ax + By + Cz + D = 0 */
    class Plane {
    public:
        float A;
        float B;
        float C;
        float D;

        Plane(Point& a, Point& b, Point& c);
        Plane(float A, float B, float C, float D);

        float dist(Point& point);
        float get_z(float x, float y);
    };



    class RANSAC {
        int num_iters = 50;
        public:
        Plane run(std::vector<Point> &points);
    };
};
