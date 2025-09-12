/*
相机标定部分
有点乱，但能用就不想整理了（
*/

#include "calibrate_gkd.hpp"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

#include <thread>

#include "calibrate_parameter_gkd.hpp"

// #define IMG_COUNT 60
// #define SAMPLE_PERIOD 40

int IMG_COUNT = 60;
int SAMPLE_PERIOD = 40;

/*相机标定参数*/
camera_cali_type camera;

//json
using json = nlohmann::json;
void writeCameraParametersToJson(const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const std::string& filename);
void readCameraParametersFromYaml(const std::string& filename, cv::Mat& cameraMatrix, cv::Mat& distCoeffs);
int calibration_main();
long int get_sysetm_time_ms();
cv::Mat rect_cut(cv::Mat image);

/*************** calibration settings ****************/
cv::Size boardSize(params.boardSize_h, params.boardSize_w); // 棋盘格的尺寸
float squareSize = params.squareSize; // 每格的大小，单位mm

/*可视化线程*/
int visulization_task()
{
    cv::Mat frame;
    while(1)
    {
        
        HIKframemtx.lock();
        HIKimage.copyTo(frame);
        HIKframemtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if(frame.empty()) continue;
        if(cv::waitKey(1) == 'q') break;
        
        if(params.rect_cut == 1){
            frame = rect_cut(frame);
        }

        if(get_sysetm_time_ms() - camera.plate_lasttime < 10)
        {
            cv::drawChessboardCorners(frame, boardSize, camera.corners, true);
        }

        //img count
        if(camera.image_sample_isok == 0)
        {
            char text[50];
            std::sprintf(text, "image:%d/%ld", IMG_COUNT, camera.frame_calib.size());
            cv::putText(frame, text, cv::Point(0,30)
                , cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0,0,255), 3);
        }else{
            char text[50];
            std::sprintf(text, "OK");
            cv::putText(frame, text, cv::Point(0,60)
                , cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0,255,0), 6);
        }

        cv::imshow("cam", frame);
        // cv::imwrite("../cam.jpg",frame);
        // std::this_thread::sleep_for(std::chrono::milliseconds(30));


    }
    cv::destroyAllWindows();
    return 0;
}

int calibration_main()
{

    boardSize = cv::Size(params.boardSize_h, params.boardSize_w);
    squareSize = params.squareSize;

    std::cout << "size:" << params.boardSize_h << "x" << params.boardSize_w << std::endl;
    std::cout << "board size:" << params.squareSize << std::endl;
    std::cout << "save to:" << params.calib_yaml_path << std::endl;


    IMG_COUNT = params.img_count;
    SAMPLE_PERIOD = params.sample_period;

    cv::Mat mtx, dist;
    std::vector<cv::Mat> rvecs, tvecs;
    
    // 创建棋盘格的3D点
    std::vector<cv::Point3f> objP;
    for (int i = 0; i < boardSize.height; ++i) {
        for (int j = 0; j < boardSize.width; ++j) {
            objP.push_back(cv::Point3f(j * squareSize, i * squareSize, 0));
        }
    }


    //可视化线程
    std::thread visulizer(visulization_task);

    while(1)
    {
        //读取相机帧
        HIKframemtx.lock();
        HIKimage.copyTo(camera.frame);
        HIKframemtx.unlock();
        if(camera.frame.empty()) continue;
        
        if(params.rect_cut == 1){
            camera.frame = rect_cut(camera.frame);
        }

        //检测棋盘格角点
        cv::cvtColor(camera.frame, camera.frame_gray, cv::COLOR_BGR2GRAY);
        if(!cv::findChessboardCorners(camera.frame_gray, boardSize, camera.corners)){
            continue;
        }else{
            camera.plate_lasttime = get_sysetm_time_ms();
        }
        //绘制角点
        // cv::drawChessboardCorners(camrea.frame, boardSize, camrea.corners, true);
        // cv::imshow("cam", camrea.frame);

        /*截取足够的样本帧用于相机校准*/
        camera.sample_period_count ++;
        if(camera.sample_period_count >= SAMPLE_PERIOD){
            cv::cornerSubPix(camera.frame_gray, camera.corners, cv::Size(11, 11), cv::Size(-1, -1),
                cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
            camera.imagePoints.push_back(camera.corners);
            camera.objectPoints.push_back(objP);

            //get calibration imgs
            camera.sample_period_count = 0;
            camera.frame_calib.push_back(camera.frame);
            printf("get image, idx = %ld / %d\n", camera.frame_calib.size(), IMG_COUNT);
            if(camera.frame_calib.size() >= (IMG_COUNT-1)){
                break;
            }
        }

    }

    camera.image_sample_isok = 1;

    // 执行相机标定
    cv::calibrateCamera(camera.objectPoints, camera.imagePoints, cv::Size(camera.frame_gray.rows, camera.frame_gray.cols), 
                        mtx, dist, rvecs, tvecs);

    // 输出相机参数
    std::cout << "save to:" << params.calib_yaml_path  << std::endl;
    writeCameraParametersToJson(mtx, dist, params.calib_yaml_path);
    std::cout << "Camera Matrix:\n" << mtx << std::endl;
    std::cout << "Distortion Coefficients:\n" << dist << std::endl;

    return 0;
}

// 将cameraMatrix和distCoeffs写入到YAML文件
void writeCameraParametersToJson(const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const std::string& filename) {
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);

    // 保存矩阵到文件
    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs.release();
}


// 从YAML文件读取cameraMatrix和distCoeffs
void readCameraParametersFromYaml(const std::string& filename, cv::Mat& cameraMatrix, cv::Mat& distCoeffs) {

    cv::FileStorage fs(filename, cv::FileStorage::READ);
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
}

//获取系统时间戳
long int get_sysetm_time_ms()
{
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<long int>(duration.count());
}

cv::Mat rect_cut(cv::Mat image)
{
    // 原始尺寸
    int width = image.cols;   // 1440
    [[maybe_unused]] int height = image.rows;  // 1080

    // 计算裁剪区域 (居中裁剪1080x1080)
    int cropSize = image.rows;
    int x = (width - cropSize) / 2;  // 计算左上角x坐标
    int y = 0;                       // 由于高度本身就是1080，无需调整

    // 定义裁剪矩形
    cv::Rect roi(x, y, cropSize, cropSize);

    // 进行裁剪
    cv::Mat croppedImage = image(roi).clone(); 
    return croppedImage;
}




void camrea_calibtation::init(){
    /*************** cam settings ****************/
    camrea.cam.open(camrea.cam_id, cv::CAP_V4L2);
    camrea.cam.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    camrea.cam.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    camrea.cam.set(cv::CAP_PROP_FPS, 60);
    camrea.cam.set(cv::CAP_PROP_TEMPERATURE, 5000);
    // std::cout << cam.get(cv::CAP_PROP_EXPOSURE) << std::endl;
    camrea.cam.set(cv::CAP_PROP_EXPOSURE, 100);
    camrea.cam.read(camrea.frame);
    boardSize.height = params.boardSize_h;
    boardSize.width = params.boardSize_w;

    // 创建棋盘格的3D点
    std::vector<cv::Point3f> objP;
    for (int i = 0; i < boardSize.height; ++i) {
        for (int j = 0; j < boardSize.width; ++j) {
            objP.push_back(cv::Point3f(j * squareSize, i * squareSize, 0));
        }
    }
}

int camrea_calibtation::image_add(cv::Mat frame){

   //get calibration corners
    cv::cvtColor(camrea.frame, camrea.frame_gray, cv::COLOR_BGR2GRAY);

    if(!cv::findChessboardCorners(camrea.frame_gray, boardSize, corners)){
        return -1;
    }
    //draw corners
    cv::drawChessboardCorners(camrea.frame, boardSize, corners, true);

    /*Get sample images for calibration*/
    camrea.sample_period_count ++;
    if(camrea.sample_period_count >= SAMPLE_PERIOD){
        cv::cornerSubPix(camrea.frame_gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
            cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
        imagePoints.push_back(corners);
        objectPoints.push_back(objP);

        //get calibration imgs
        camrea.sample_period_count = 0;
        camrea.frame_calib.push_back(camrea.frame);
        printf("get image, idx = %ld / %d\n", camrea.frame_calib.size(), IMG_COUNT);
        if(camrea.frame_calib.size() >= (IMG_COUNT-1)){
            return 0;
        }
    }

}