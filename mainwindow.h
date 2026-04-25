#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "DownloadManager.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>

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

public slots: //槽函数

private:
    Ui::MainWindow *ui;
    DownloadManager downloadManager;  //实例化一个DownloadManager对象
    QLineEdit *urlEdits[4];
    QPushButton *downloadBtns[4];


};
#endif // MAINWINDOW_H
