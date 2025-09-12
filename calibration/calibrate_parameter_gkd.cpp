#include "calibrate_parameter_gkd.hpp"

int yaml_write(const std::string& filepath, const parameter_loader_t& params);
int yaml_load(const std::string& filepath, parameter_loader_t& params);
int para_load(std::string filepath);

parameter_loader_t params;

int para_load(std::string filepath)
{

    // debug 写入以生成yaml文件
    // yaml_write(filepath, params);
    // return 0;
    yaml_load(filepath, params);
    // std::cout << params.armor_small_h << std::endl;

}

// 保存参数到 YAML 文件
int yaml_write(const std::string& filepath, const parameter_loader_t& params) {
    cv::FileStorage fs(filepath, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "错误：无法打开文件 " << filepath << " 用于写入！" << std::endl;
        return -1;
    }
    /*标定参数*/
    fs << "boardSize_h" << params.boardSize_h;
    fs << "boardSize_w" << params.boardSize_w;
    fs << "squareSize" << params.squareSize;
    fs << "img_count" << params.img_count;
    fs << "sample_period" << params.sample_period;
    fs << "calib_yaml_path" << params.calib_yaml_path;
    fs << "rect_cut" << params.rect_cut;
    fs << "cam_gain"         << params.cam_gain;
    fs << "cam_exptime"      << params.cam_exptime;
    fs << "framerate"        << params.framerate;

    fs.release();
    std::cout << "参数已保存到 " << filepath << std::endl;
    return 0;
}


// 我感觉在相机标定中应该用不到？
// 从 YAML 文件加载参数
int yaml_load(const std::string& filepath, parameter_loader_t& params) {
    cv::FileStorage fs(filepath, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "错误：无法打开文件 " << filepath << " 用于读取！" << std::endl;
        return -1;
    }
    /*标定参数*/
    fs["boardSize_h"] >> params.boardSize_h;
    fs["boardSize_w"] >> params.boardSize_w;
    fs["squareSize"] >> params.squareSize;
    fs["img_count"] >> params.img_count;
    fs["sample_period"] >> params.sample_period;
    fs["calib_yaml_path"] >> params.calib_yaml_path;

    fs["cam_gain"] >> params.cam_gain;
    fs["cam_exptime"] >> params.cam_exptime;
    fs["framerate"] >> params.framerate;

    fs.release();
    std::cout << "参数已从 " << filepath << " 加载" << std::endl;
    return 0;
}