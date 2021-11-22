#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "socket.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_brightness_slid_valueChanged(int value);

    void on_contrast_slid_valueChanged(int value);

    void on_pushButton_2_clicked();

private:
    //MainWindow object member variables
    Ui::MainWindow *ui;
    socket *cTest;
    double contrastdpi = 10.0;
};
#endif // MAINWINDOW_H
