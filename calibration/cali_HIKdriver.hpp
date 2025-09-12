#ifndef HIK_CPP
#define HIK_CPP

#include <iostream>
#include <X11/Xlib.h> 
#include <assert.h>  
#include "math.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include "include/MvCameraControl.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <pthread.h>

extern cv::Mat HIKimage;
extern std::mutex HIKframemtx;

int HIKcamtask();

#endif