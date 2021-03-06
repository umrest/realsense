#include "RealsenseDriver.h"
#include "ContourDetector.h"

#include <vector>
#include <comm/CommunicationDefinitions.h>
#include <comm/Realsense_Command.h>

cv::Point2f get_actual_point(cv::Point2f point, int width, int height){
    cv::Point2f ret;
    ret.x = point.x - width/2;
    ret.y = -point.y + height;

    return ret;
}
RealsenseDriver::RealsenseDriver() : client(comm::CommunicationDefinitions::IDENTIFIER::REALSENSE){

}
void get_scaled(Mat& img, Mat& dst){
    double min;
    double max;
    cv::minMaxIdx(img, &min, &max);
    cv::convertScaleAbs(img, dst, 255 / max);
}

void RealsenseDriver::send_image(Mat &img)
	{
		// send image to dashboard
		std::vector<int> param(2);
		param[0] = cv::IMWRITE_JPEG_QUALITY;
		param[1] = 2;
		cv::imencode(".jpg", img, buffer, param);
		//cout << buffer.size() << endl;


		vector<uint8_t> type(1);
		type[0] = (uint8_t)comm::CommunicationDefinitions::TYPE::VISION_IMAGE;

		int sz = comm::CommunicationDefinitions::PACKET_SIZES.at(comm::CommunicationDefinitions::TYPE::VISION_IMAGE) + 1;
		//cout << sz << endl;
		std::vector<uchar> buf(sz);
		buf[0] = (char)comm::CommunicationDefinitions::TYPE::VISION_IMAGE;
		std::copy(buffer.begin(), buffer.end(), buf.begin() + 1);
		client.write(buf);
    }

void RealsenseDriver::run(){
    ContourDetector contour_detector;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;

    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, 480, 270, RS2_FORMAT_Z16, 15); // Enable default depth

    auto profile = pipe.start(cfg);
    auto intrin = profile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();
    float scale = profile.get_device().first<rs2::depth_sensor>().get_depth_scale();
    //Our goal is to generate depth without any holes, since these are going to pose an immediate problem to our algorithm. The best way to reduce the number of missing pixels is by letting the hardware do it. The D400 cameras have a High Density preset we can take advantage of.

    // Set the device to Default preset
    auto sensor = profile.get_device().first<rs2::depth_sensor>();
    sensor.set_option(RS2_OPTION_VISUAL_PRESET, RS2_RS400_VISUAL_PRESET_DEFAULT);

    int i = 0;

    Projection p(55, .5, &intrin , scale);

    while (true)
    {
        // Block program until frames arrive
        rs2::frameset frames = pipe.wait_for_frames();

        // Try to get a frame of a depth image
        rs2::frame depth = frames.get_depth_frame();
    
        // Query frame size (width and height)
        int w = depth.as<rs2::video_frame>().get_width();
        int h = depth.as<rs2::video_frame>().get_height();

        // get depth map and convert it to 8bit
        Mat depth_image(Size(w, h), CV_16UC1, (void *)depth.get_data(), Mat::AUTO_STEP);

        Mat projected;
        p.project(depth_image, projected);

        plane_extractor.run(projected);

        Mat scaled;
        get_scaled(projected, scaled);

        Mat obstacle_mask;
        threshold( projected, obstacle_mask, 1, 255, cv::THRESH_BINARY );

        cv::erode(obstacle_mask, obstacle_mask, Mat());
        cv::dilate(obstacle_mask, obstacle_mask, Mat());

        std::vector<Contour> contours = contour_detector.run(obstacle_mask);

std::vector<comm::Obstacle*> obstacles;
        obstacles.push_back(&realsense._obstacle_1);
        obstacles.push_back(&realsense._obstacle_2);
        obstacles.push_back(&realsense._obstacle_3);
        obstacles.push_back(&realsense._obstacle_4);
        
        for(int i = 0; i < obstacles.size(); i++){

            if(contours.size() > i){
                obstacles[i]->set_x((contours[i].x - 100) / 2.54);
                obstacles[i]->set_y((200-contours[i].y)/ 2.54);
                obstacles[i]->set_width ( contours[i].radius*2 / 2.54);
                obstacles[i]->set_height( contours[i].radius*2 / 2.54);
            }
            else{
                obstacles[i]->set_x(0);
                obstacles[i]->set_y(0);
                obstacles[i]->set_width (0);
                obstacles[i]->set_height(0);
            }


        } 

        auto messages = client.get_messages();
			for(auto &m : messages){
				if(m->type() == comm::CommunicationDefinitions::TYPE::REALSENSE_COMMAND){
					comm::Realsense_Command* command = (comm::Realsense_Command*)m.get();
					if(command->get_command() == 10){
						send_image(obstacle_mask);
					}
					if(command->get_command() == 9){
                        send_image(projected);
					}
				}
			}
    
      
			  client.send_message(&realsense);

    }
}
