#pragma once

#include "Projection.h"

#include <comm/RESTClient.h>

#include <comm/Realsense.h>

#include "PlaneExtractor.h"

class RealsenseDriver
{
    comm::RESTClient client;
	std::vector<uchar> buffer;
    void send_image(Mat &img);

    comm::Realsense realsense;

    PlaneExtractor plane_extractor;
    public:
    void run();

    RealsenseDriver();

};