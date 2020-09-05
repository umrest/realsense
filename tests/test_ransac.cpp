#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include "RANSAC.h"
#include <iostream>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_dist, * boost::unit_test::tolerance(0.00001) ){
    realsense::Plane plane(2, 4, 3, 5);

    realsense::Point point(1, 2, 3);

    BOOST_TEST(plane.dist(point) == 4.45668793);

    realsense::Plane plane2(0, 0.5, 0.5, 0.5);

    realsense::Point point2(1, 0, 0);

    BOOST_TEST(plane2.dist(point2) == 0.707106769);
}

BOOST_AUTO_TEST_CASE( test_basic ){
    realsense::RANSAC ransac;
    std::vector<realsense::Point> pts;
    pts.push_back(realsense::Point(1, 2, 3));
    pts.push_back(realsense::Point(3, -2, 1));
    pts.push_back(realsense::Point(2, -1, -3));

    auto plane = ransac.run(pts);

    BOOST_TEST(plane.A == 18);
    BOOST_TEST(plane.B == 10);
    BOOST_TEST(plane.C == -2);
    BOOST_TEST(plane.D == -32);
}

BOOST_AUTO_TEST_CASE( test_one_point_not_in_plane ){
    realsense::RANSAC ransac;
    std::vector<realsense::Point> pts;
    pts.push_back(realsense::Point(1, 0, 0));
    pts.push_back(realsense::Point(0, 1, 0));
    pts.push_back(realsense::Point(0, 0, 1));
    pts.push_back(realsense::Point(0.5, 0.5, 0.5));

    auto plane = ransac.run(pts);

    BOOST_TEST(plane.A == 1);
    BOOST_TEST(plane.B == 1);
    BOOST_TEST(plane.C == 1);
    BOOST_TEST(plane.D == -1);
}


