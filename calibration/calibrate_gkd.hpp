#ifndef CALIB_H
#define CALIB_H


#include <iostream>

// opencv
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

// cam_driver
#include "cali_HIKdriver

typedef struct
{
    cv::Mat frame;
    cv::Mat frame_gray;
    std::vector<cv::Mat> frame_calib;
    cv::VideoCapture cam;
    int sample_period_count = 0;
    int cam_id = 0;             //相机ID
    long int plate_lasttime = 0;    //上次检测到标定板的时间
    int image_sample_isok = 0;
    std::string video_path;     //或，视频文件路径
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;
    std::vector<cv::Point2f> corners;
    std::vector<cv::Point3f> objP;
}camera_cali_type;

class camrea_calibtation
{
private:
    /* data */
public:
    camera_cali_type camrea;
    cv::Mat mtx, dist;
    std::vector<cv::Mat> rvecs, tvecs;

    /*************** calibration settings ****************/
    //TODO
    cv::Size boardSize; // 棋盘格的尺寸
    float squareSize = 20.0f; // 每格的大小，单位mm
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;
    std::vector<cv::Point2f> corners;
    // 创建棋盘格的3D点
    std::vector<cv::Point3f> objP;
    
    void init();
    int image_add(cv::Mat frame);

    camrea_calibtation(/* args */);
    ~camrea_calibtation();
};




extern int calibration_main();
extern cv::Mat rect_cut(cv::Mat image);
#endif