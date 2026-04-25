#include "DownloadManager.h"
#include <thread>
#include <iostream>
#include <curl/curl.h>

static std::mutex g_consoleMutex; //终端打印锁，防止同时写入
int DownloadManager::progressCallback(void* clientp, 
                                      curl_off_t dltotal, curl_off_t dlnow,
                                      curl_off_t ultotal, curl_off_t ulnow){
    
    auto* pData = static_cast<CallbackData*>(clientp);
    
    if (!pData) return 0;
    
    // 检查是否要取消
    if (pData->cancelFlag && pData->cancelFlag->load())
        return 1;  // 返回 1 表示中断
    
    // 更新进度
    if (dltotal > 0) {
        int progress = (int)((dlnow * 100) / dltotal);
        std::lock_guard<std::mutex> lock(pData->mgr->mtx);
        pData->mgr->taskMap[pData->taskId].progress = progress;
    }
    
    return 0;

}

DownloadManager::DownloadManager(const size_t maxConcurrent):maxConcurrent(maxConcurrent){
    if(maxConcurrent > 4)
        throw std::runtime_error("maxConcurrent 超出最大同时下载数量");
    else {
        std::lock_guard<std::mutex> lock(g_consoleMutex);
        std::cout << "maxConcurrent = " << maxConcurrent << std::endl;
    }
    //创建线程
    for(int i=0;i<maxConcurrent;i++){
        //普通函数的名字单独出现时会隐式退化为函数指针,成员函数不会
        threads.emplace_back(&DownloadManager::loop,this);
    }
}
DownloadManager::~DownloadManager(){
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }

    con_var.notify_all(); //唤醒所有线程

    for(auto& t:threads){
        if(stop && t.joinable()){
            {
                std::lock_guard<std::mutex> lock(g_consoleMutex);
                std::cout << "delete thread ,id is " <<t.get_id()<<  std::endl;
            }
            t.join();
        }
            
    }
}

//通过URL获取文件名
std::string getFileNameFromURL(const std::string& url){
    size_t pos = url.find_last_of('/');

    //pos没找到，就返回npos
    if(pos != std::string::npos){
        //string substr(size_t pos, size_t len = npos);从pos开始截取到文件末尾
        return url.substr(pos+1);
    }
    return "downloaded_file";
}
//提交下载任务，返回任务ID
int DownloadManager::addDownload(const std::string& url){
    const std::string savePath = getFileNameFromURL(url); //
    int taskId = ++nowTaskId;                     // 生成唯一ID

    // 提前创建取消标志
    auto cancelFlag = std::make_shared<std::atomic<bool>>(false);

    // 2. 构造任务信息（存入 taskMap）
    DownloadTask task;
    task.id = taskId;
    task.url = url;
    task.savePath = savePath;
    task.status = DownloadStatus::PENDING;
    task.progress = 0;
    task.errorMessage = "";

    // 3. 构造队列任务（供工作线程取出）
    DownloadJob job;
    job.taskId = taskId;
    job.url = url;
    job.savePath = savePath;

    {
        std::lock_guard<std::mutex> lock(mtx);
        taskMap[taskId] = std::move(task);
        cancelFlags[taskId] = cancelFlag;
        tasks.push(std::move(job)); //把任务放入队列，让loop中的线程取出
    }
    //唤醒一个线程
    con_var.notify_one();

    {
        std::lock_guard<std::mutex> lock(g_consoleMutex);
        std::cout << "Download task submitted, ID: " << taskId << std::endl;
    }
    return taskId;
}

// 取消指定任务
bool DownloadManager::cancelDownload(int taskId){
        std::lock_guard<std::mutex> lock(mtx);
        auto it = cancelFlags.find(taskId);
        if(it != cancelFlags.end()){
            it->second->store(true);
            return true;
        }
        return false;
}


DownloadTask DownloadManager::getTaskStatus(int taskId){
    auto it = taskMap.find(taskId);
    if(it != taskMap.end()){
        return it->second;
    }
    else{
        {
            std::lock_guard<std::mutex> lock(g_consoleMutex);
            std::cout << "Task ID not found" << std::endl;
            std::cout << "Task ID " << taskId << " not found" << std::endl;
        }
        DownloadTask invalidTask;
        invalidTask.id = -1;
        invalidTask.status = DownloadStatus::CANCELLED;  // 或其他状态
        invalidTask.errorMessage = "Task not found";
        return invalidTask;
    }
}

std::vector<DownloadTask> DownloadManager::getAllTasks(){
    std::vector<DownloadTask> all_DownloadTask;
    std::lock_guard<std::mutex> lock(mtx);
    for(auto& task:taskMap){
        all_DownloadTask.push_back(task.second);
    }
    return all_DownloadTask;
}

// 取消所有任务
void DownloadManager::cancelAll(){
    std::lock_guard<std::mutex> lock(mtx);
    for(auto& pair:cancelFlags){
        pair.second->store(true);
    }
}

void DownloadManager::loop(){
        CURL * curl = curl_easy_init();
        if(!curl){
            {
                std::lock_guard<std::mutex> lock(g_consoleMutex);
                std::cerr <<"Failed to init CURL in thread" << std::endl;
            }
            return;
        }
        //在多线程中必须设置禁用信号，防止系统奔溃
        curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1L);

        while(true){
            //std::condition_variable::wait 必须能够临时解锁互斥量，lock_guard不能手动解锁
            std::unique_lock<std::mutex> lock(mtx);
            
            //唤醒条件
            con_var.wait(lock,[this]{
                if(tasks.empty()){
                    std::lock_guard<std::mutex> lock(g_consoleMutex);
                    std::cout << "tasks is empty" << std::endl;
                }
                return !tasks.empty() || stop;
            });

            //退出线程
            if(stop && tasks.empty()){
                {
                    std::lock_guard<std::mutex> lock(g_consoleMutex);
                    std::cout << "exit thread: " <<std::this_thread::get_id()<<  std::endl;
                }
                break;
            } 

            //检查是否有任务
            if(tasks.empty()){
                continue;
            }
            
            //取出addDownload放入的任务
            DownloadJob task = tasks.front();
            tasks.pop();

            lock.unlock();

            {
                std::lock_guard<std::mutex> lock(g_consoleMutex);
                std::cout << "Processing Task: " << task.taskId 
                          << " on Thread: " << std::this_thread::get_id() << std::endl;
            }
            
            //执行下载
            executeDownload(curl,task.taskId); 
        }
        //清理句柄
        curl_easy_cleanup(curl);
    }

    void DownloadManager::executeDownload(CURL* curl, int taskId){
        
        std::string url,savePath;
        std::shared_ptr<std::atomic<bool>> cancelFlag;
        //获取任务的信息
        {
            std::lock_guard<std::mutex> lock(mtx);
            auto it = taskMap.find(taskId);

            //任务不存在
            if(it == taskMap.end() ) return;
            
            url = it->second.url;
            savePath = it->second.savePath;
            it->second.status = DownloadStatus::DOWNLOADING; 

            //获取取消标志
            auto flagIt = cancelFlags.find(taskId); //输入任务id获取对应标志
        if(flagIt != cancelFlags.end()) //如果有，就赋值
            cancelFlag = flagIt->second;   
        else
            return;  // 理论上不会发生
        }

        //创建并打开文件
        FILE* file = fopen(savePath.c_str(),"wb");
        if(!file){
            std::lock_guard<std::mutex> lock(mtx);
            taskMap[taskId].status = DownloadStatus::CANCELLED;
            taskMap[taskId].errorMessage = "Cannot open file:"+savePath;
            {
                std::lock_guard<std::mutex> lock(g_consoleMutex);
                std::cerr << "Cannot open file: " << savePath << std::endl;
            }
            return;
        }

        //创建回调数据结构体
        CallbackData callbackData{taskId, this, cancelFlag};

        //配置curl
        curl_easy_setopt(curl,CURLOPT_URL,url.c_str()); //设置下载链接
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);      // 总超时 30 秒
        curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,10L); //10秒超时
        //curl_easy_setopt(curl,CURLOPT_VERBOSE,1L);  //详细日志
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,file); //写入文件
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &callbackData); 
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        CURLcode res = curl_easy_perform(curl); //连接服务器，开始下载，过程是阻塞进程
        fclose(file);

    // 清理标志并更新状态
    {
        std::lock_guard<std::mutex> lock(mtx);
        cancelFlags.erase(taskId);
        auto& task = taskMap[taskId];
        if (res == CURLE_ABORTED_BY_CALLBACK) {
            task.status = DownloadStatus::CANCELLED;
            task.errorMessage = "Cancelled by user";
        } else if (res == CURLE_OK) {
            task.status = DownloadStatus::COMPLETED;
            task.progress = 100;
        } else {
            task.status = DownloadStatus::CANCELLED;
            task.errorMessage = curl_easy_strerror(res);
            {
                std::lock_guard<std::mutex> lock(g_consoleMutex);
                std::cerr << task.errorMessage<< std::endl;
            }
        }
    }


    }