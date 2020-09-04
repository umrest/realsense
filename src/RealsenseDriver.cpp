#include "RealsenseDriver.h"
#include "ContourDetector.h"

#include <vector>
#include <comm/CommunicationDefinitions.h>

cv::Point2f get_actual_point(cv::Point2f point, int width, int height){
    cv::Point2f ret;
    ret.x = point.x - width/2;
    ret.y = -point.y + height;

    return ret;
}
RealsenseDriver::RealsenseDriver() : client(comm::CommunicationDefinitions::IDENTIFIER::REALSENSE){

}

void show_scaled(Mat& img){
    double min;
        double max;
        cv::minMaxIdx(img, &min, &max);
        cv::Mat adjMap;
        cv::convertScaleAbs(img, adjMap, 255 / max);
        
        cv::imshow("aa", adjMap);
        
        cv::waitKey(100);
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


        // Generate mask to fill in holes



        Mat dst;
        Projection p(55, .5, &intrin , scale);
        p.project(depth_image, dst);

        Mat mask;
        threshold( dst, mask, 0, 255, 1 );

        Mat mask_inv = ~mask;

        cv::Scalar mean2 = cv::mean(dst, mask_inv);

        int mean = (int)mean2[0];

        Mat dst_filled = dst.setTo(mean, mask);


        
        // Anything 15 cemeters above the ground( the mean)
        int threshold2 = mean - .05 * 100;

        Mat mask2;
        threshold( dst_filled, mask2, threshold2, 255, 1 );

        

        Mat mask_test = mask2 & mask_inv;

        cv::erode(mask_test, mask_test, Mat());
        cv::dilate(mask_test, mask_test, Mat());

        //show_scaled(dst);

        send_image(mask_test);

        std::vector<Contour> contours = contour_detector.run(mask_test);
      std::cout << contours.size() << std::endl;

          realsense._obstacle_1.set_x(0);
          realsense._obstacle_1.set_y(0);
          realsense._obstacle_1.set_width(0);
          realsense._obstacle_1.set_height(0);

          realsense._obstacle_2.set_x(0);
          realsense._obstacle_2.set_y(0);
          realsense._obstacle_2.set_width(0);
          realsense._obstacle_2.set_height(0);

          realsense._obstacle_3.set_x(0);
          realsense._obstacle_3.set_y(0);
          realsense._obstacle_3.set_width(0);
          realsense._obstacle_3.set_height(0);

          realsense._obstacle_4.set_x(0);
          realsense._obstacle_4.set_y(0);
          realsense._obstacle_4.set_width(0);
          realsense._obstacle_4.set_height(0);
          
        if(contours.size() > 0){
          realsense._obstacle_1.set_x((contours[0].x - 100) / 2.54);
          realsense._obstacle_1.set_y((200-contours[0].y)/ 2.54);
          realsense._obstacle_1.set_width ( contours[0].radius*2 / 2.54);
          realsense._obstacle_1.set_height( contours[0].radius*2 / 2.54);
        }
      

			  client.send_message(&realsense);
        
    }
    }
