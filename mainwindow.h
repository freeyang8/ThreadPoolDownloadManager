#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "DownloadManager.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
signals: //信号

private slots: //槽函数
    void updateProgressBars(); //定时轮询，更新下载进度条
private:
    Ui::MainWindow *ui;
    DownloadManager downloadManager;  //实例化一个DownloadManager对象
    QLineEdit *urlEdits[4];
    QPushButton *downloadBtns[4];
    QProgressBar *progressBars[4];
    int taskIds[4];
    QTimer *progressTimer;

};
#endif // MAINWINDOW_H
