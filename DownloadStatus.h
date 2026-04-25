#pragma once

enum DownloadStatus{
    PENDING,     // 等待中
    DOWNLOADING, // 下载中
    COMPLETED,   // 已完成
    CANCELLED    // 已取消
};
