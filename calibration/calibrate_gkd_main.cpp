#include <iostream>

// 引入标定头文件
#include "cali_HIKdriver.hpp"
#include "calibrate_parameter_gkd.hpp"
#include "calibrate_gkd.hpp"

// 引入线程
#include <thread>
#include <openvino/openvino.hpp>
#include <thread>


int main(int argc, char** argv) {
    
    para_load("../configs/gkd_calibrate_configs.yaml");

    std::thread cameraThread(HIKcamtask);
    cv::Mat inputImage;

    if(argc > 1)
    {
        std::string command_str;
        command_str = argv[1];    
        std::cout << command_str << std::endl;
        if(command_str == "--calibration")
        {
            //相机标定
            std::cout << "into camera calibration ....." << std::endl;
            calibration_main();
            return 0;
        }else{
            std::cout << "usage: " << std::endl;
            std::cout << "--calibration -> to calibrate camera" << std::endl;
        }
    }

}