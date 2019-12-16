class ContourDetector {
	public:
    void run(cv::Mat &img, cv::Mat &contours){
        cv::Canny(img,contours,35,90);
    }
};