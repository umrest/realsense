#include "Projection.h"

void Projection::project(Mat &src, Mat &dst)
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