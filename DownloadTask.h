#pragma once
#include <string>
#include "DownloadStatus.h"

// 下载任务信息
struct DownloadTask {
    int id;
    std::string url;
    std::string savePath;
    DownloadStatus status;  //状态
    int progress;  // 0-100
    std::string errorMessage; //错误信息
};