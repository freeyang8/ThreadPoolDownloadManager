#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>

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

    for(const QString &text: buttonTexts){
        //创建水平布局
        QHBoxLayout* rowLayout = new QHBoxLayout;

        //按钮
        QPushButton* btn = new QPushButton(text);

        //URL输入框
        QLineEdit* lineEdit = new QLineEdit;

        //把按钮和输入框加入水平布局中
        rowLayout->addWidget(btn);
        rowLayout->addWidget(lineEdit);


        download_layout ->addLayout(rowLayout);
    }

    //中央布局
    centralWidget()->setLayout(download_layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
