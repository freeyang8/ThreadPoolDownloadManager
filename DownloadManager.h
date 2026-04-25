#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <unordered_map>
#include <iostream>
#include <curl/curl.h>
#include "DownloadTask.h"

//任务结构体
struct DownloadJob{
    int taskId;
    std::string url;
    std::string savePath;
};

// 下载管理器
class DownloadManager {
public:
    DownloadManager(size_t maxConcurrent = 4);
    ~DownloadManager();
    
    // 提交下载任务，返回任务ID
    int addDownload(const std::string& url, const std::string& savePath);
    
    // 取消指定任务
    bool cancelDownload(int taskId);
    
    // 取消所有任务
    void cancelAll();
    
    // 查询任务状态
    DownloadTask getTaskStatus(int taskId);
    
    // 获取所有任务状态
    std::vector<DownloadTask> getAllTasks();
    
    
private:
    //将线程挂起，让它等待任务
    void loop();

    // 执行实际下载
    void executeDownload(CURL* curl, int taskId);

    //回调数据
    struct CallbackData {
        int taskId;
        DownloadManager* mgr;
        std::shared_ptr<std::atomic<bool>> cancelFlag;
    };

   const size_t maxConcurrent;
   std::vector<std::thread> threads; //存放线程 
   std::mutex mtx;
   bool stop = false;
   std::queue<DownloadJob> tasks;  //存放任务
   std::unordered_map<int,DownloadTask>taskMap; //存储任务信息
   std::condition_variable con_var;
   std::atomic<int> nowTaskId{0}; //返回的id

   //任务取消标志映射表：taskId -> 原子布尔指针，用于线程安全的任务取消
   std::unordered_map<int,std::shared_ptr<std::atomic<bool>>> cancelFlags;
   //回调函数，实时显示下载进度
   static int progressCallback(void* clientp,
                                curl_off_t dltotal,curl_off_t dlnow,
                                curl_off_t ultotal,curl_off_t ulnow);
};