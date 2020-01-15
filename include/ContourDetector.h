class ContourDetector
{
private:
public:
    void run(cv::Mat &img, cv::Mat &contour_image)
    {
        Mat canny_output;
        cv::Canny(img, canny_output, 2, 12);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        contour_image = Mat::zeros(canny_output.size(), CV_8UC3);

        for (int i = 0; i < contours.size(); i++)
        {
            Scalar color = Scalar(255, 0, 0);
            drawContours(contour_image, contours, i, color, 2, 8, hierarchy, 0, Point());
        }
    }
};