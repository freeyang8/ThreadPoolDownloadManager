#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QDir>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //窗口名称
    setWindowTitle("多线程下载器");

    //添加4个按钮，点击则进行下载
    QVBoxLayout *download_layout = new QVBoxLayout;

    QStringList buttonTexts = {"开始下载1","开始下载2","开始下载3","开始下载4"};

    for(int i=0;i<4;i++){
        QHBoxLayout *rowLayout = new QHBoxLayout; //创建水平布局

        downloadBtns[i] = new QPushButton(buttonTexts[i]);
        urlEdits[i] = new QLineEdit;
        urlEdits[i]->setPlaceholderText("请输入URL");

        progressBars[i] = new QProgressBar;
        progressBars[i]->setRange(0,100);
        progressBars[i]->setValue(0);

        taskIds[i] = 0;

        rowLayout->addWidget(downloadBtns[i]); //加入按钮
        rowLayout->addWidget(urlEdits[i]);      //加入输入框

        download_layout->addLayout(rowLayout); //把水平布局 塞入 整体垂直 布局中
        download_layout->addWidget(progressBars[i]); //垂直加入进度条

        //点击按钮，进行传输url，进行下载
        connect(downloadBtns[i],&QPushButton::clicked,[this,i]{
            downloadBtns[i]->setEnabled(false); //不可重复点击下载按钮

            QString url = urlEdits[i]->text();//获取输入框的内容
            if(url.isEmpty()){
                return;
            }

            //文件保存路径
            QString init_savePath = "../../download/";

            int taskid = downloadManager.addDownload(url.toStdString(),init_savePath.toStdString()); //转为std的字符串进行传输
            taskIds[i] = taskid;
            progressBars[i]->setValue(0); //重置进度条
        });
    }

    //中央布局
    centralWidget()->setLayout(download_layout);

    //创建计时器获取下载进度
    progressTimer = new QTimer(this);
    connect(progressTimer, &QTimer::timeout, this, &MainWindow::updateProgressBars);
    progressTimer->start(100);
    // // 获取当前目录
    // QString cur_dir = QDir::currentPath();
    // qDebug() << "当前目录:" << cur_dir;

    // // 获取上一级目录
    // QDir parentDir = QDir(cur_dir);
    // parentDir.cdUp();  // 进入上一级
    // QString parentPath = parentDir.absolutePath();

    // qDebug() << "上一级目录:" << parentPath;
}

MainWindow::~MainWindow()
{
    delete ui;
}

//定时轮询，更新下载进度条
void MainWindow::updateProgressBars(){
    for(int i=0;i<4;i++){
        int id = taskIds[i];
        if(id <= 0) continue; //没有下载任务

        //得到下载的任务信息
        DownloadTask task = downloadManager.getTaskStatus(id);
        if(task.id == -1 ){
            taskIds[i] = 0;     // 任务已失效，解除绑定
            downloadBtns[i]->setEnabled(true); //重置按钮状态
            continue;
        }

        progressBars[i]->setValue(task.progress);

        if (task.status == DownloadStatus::COMPLETED ||
            task.status == DownloadStatus::CANCELLED) {
            taskIds[i] = 0;     // 任务结束，停止更新，保持当前进度
            downloadBtns[i]->setEnabled(true); //重置按钮状态
        }
    }
}

