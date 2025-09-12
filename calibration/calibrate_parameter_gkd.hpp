#ifndef LOADER_CPP
#define LOADER_CPP

#ifdef Status
#undef Status
#endif

#include <iostream>
// opencv
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>


typedef struct {
    /*标定参数*/
    int boardSize_h;        //ok
    int boardSize_w;        //ok
    float squareSize;       //ok
    int img_count;          //ok
    int sample_period;      //ok
    std::string calib_yaml_path;    //ok

} parameter_loader_t;

extern int para_load(std::string filepath);

extern parameter_loader_t params;

#endif
