#pragma once

#include "Projection.h"

#include <comm/RESTClient.h>

#include <comm/Realsense.h>

class RealsenseDriver
{
comm::RESTClient client;
	std::vector<uchar> buffer;
    void send_image(Mat &img);

    comm::Realsense realsense;
    public:
    void run();

    RealsenseDriver();

};