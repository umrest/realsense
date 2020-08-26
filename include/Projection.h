#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <opencv2/features2d.hpp>

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <librealsense2/rsutil.h>

using namespace std;
using namespace cv;


struct Projection
{
private:
    double angle;                        // in radians
    double scale;
    double height;                       // in meters
    const struct rs2_intrinsics *intrin; // camera parameters
    struct rs2_extrinsics extrin;

public:
    Projection(double _angle, double _height, const struct rs2_intrinsics *_intrin, float _scale)
        : angle(_angle * 3.14159 / 180.0), height(_height), intrin(_intrin), scale(_scale)
    {
        extrin.translation[0] = 0;
        extrin.translation[1] = 1;
        extrin.translation[2] = 0;

        extrin.rotation[0] =1;
        extrin.rotation[1] = 0;
        extrin.rotation[2] = 0;
        extrin.rotation[3] = 0;
        extrin.rotation[4] = cos(-angle);
        extrin.rotation[5] = -sin(-angle);
        extrin.rotation[6] = 0;
        extrin.rotation[7] = sin(-angle);
        extrin.rotation[8] = cos(-angle);
    }

    void project(Mat &src, Mat &dst)
    {
        dst = cv::Mat::zeros(cv::Size(200,200), CV_8UC1);

        for (int y = 0; y < src.rows; y += 1)
        {
            // Loop over all columns
            for (int x = 0; x < src.cols; x += 1)
            {
                float pixel[2];
                pixel[0] = x;
                pixel[1] = y;
                float  depth = src.at<uint16_t>(y,x) * scale;

               // cout << pixel[0] << " " << pixel[1] << " " << depth << endl;

                float pt_in[3];
                rs2_deproject_pixel_to_point(pt_in, intrin, pixel, depth);

                //cout << pt_in[0] << " " << pt_in[1] << " " << pt_in[2] << endl;

                float pt_out[3];

                rs2_transform_point_to_point(pt_out, &extrin, pt_in);

               // cout << pt_out[0] << " " << pt_out[1] << " " << pt_out[2] << endl;

                float x_2 = pt_out[0];
                float y_2 = pt_out[1];
                float z_2 = pt_out[2];

                int x_l = (1+x_2) * dst.size().height/2;
                int y_l = (1+y_2) * dst.size().width/2;

                if(x_l >= 0 && x_l < dst.size().width && y_l >= 0 && y_l < dst.size().height){
                    dst.at<uint8_t>(y_l, x_l) = (z_2) * 100;
                }
            }
        }
    }
};


void show_scaled(Mat& img){
    double min;
        double max;
        cv::minMaxIdx(img, &min, &max);
        cv::Mat adjMap;
        cv::convertScaleAbs(img, adjMap, 255 / max);
        
        cv::imshow("aa", adjMap);
        
        cv::waitKey(100);
}