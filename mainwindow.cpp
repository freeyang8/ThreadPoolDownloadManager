#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QDir>
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

        rowLayout->addWidget(downloadBtns[i]); //加入按钮
        rowLayout->addWidget(urlEdits[i]);      //加入输入框

        download_layout->addLayout(rowLayout); //把水平布局 塞入 整体垂直 布局中

        //点击按钮，进行传输url，进行下载
        connect(downloadBtns[i],&QPushButton::clicked,[this,i]{
            QString url = urlEdits[i]->text();//获取输入框的内容
            if(url.isEmpty()){
                return;
            }
            QString init_savePath = "../../download/";
            downloadManager.addDownload(url.toStdString(),init_savePath.toStdString()); //转为std的字符串进行传输
        });
    }

    //中央布局
    centralWidget()->setLayout(download_layout);

    // 获取当前目录
    QString cur_dir = QDir::currentPath();
    qDebug() << "当前目录:" << cur_dir;

    // 获取上一级目录
    QDir parentDir = QDir(cur_dir);
    parentDir.cdUp();  // 进入上一级
    QString parentPath = parentDir.absolutePath();

    qDebug() << "上一级目录:" << parentPath;
}

MainWindow::~MainWindow()
{
    delete ui;
}
