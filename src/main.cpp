#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/features2d.hpp>

using namespace std;
using namespace cv;

int main(){

cv::VideoCapture cap;
	cap.open("/dev/v4l/by-id/usb-Intel_R__RealSense_TM__Depth_Camera_415_Intel_R__RealSense_TM__Depth_Camera_415-video-index3");

	
	cv::Mat img;

	// Setup SimpleBlobDetector parameters.
    SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 200;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 1500;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create();

   
	while (true)
	{

		if (cap.read(img))
		{

			std::vector<KeyPoint> keypoints;
			 // Detect blobs
    		detector->detect(img, keypoints);
			drawKeypoints( img, keypoints, img, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

			cv::imshow("Captured", img);
			cv::waitKey(1);
		}
		else
		{
			cout << "Unable to capture from video device" << endl;
			break;
		}
	}
}	