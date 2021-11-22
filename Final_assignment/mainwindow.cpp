#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socket.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//initialize socket object
    cTest = new socket(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString filter = "Bitmap File (*.bmp)"; //we creat a filter that will only let us select bmp files
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", "C:/", filter); //using the QFileDialog class we creat
    //a string that contains the file path of the user selected bmp image

    QPixmap pix(file_name); //we creat a pixmap using the file path
    int w = ui->label->width();
    int h = ui->label->height();
    ui->lineEdit->setText(file_name);
    ui->label->setPixmap(pix.scaled(w, h,Qt::KeepAspectRatio));
}


void MainWindow::on_brightness_slid_valueChanged(int value) //This function is used to send brightness value to server
{
    // calculate contrast and brightness value from horizontal sliding bar
    double constest = ui->contrast_slid->value()/contrastdpi;
    double brightness = double(ui->brightness_slid->value());
    cTest->sendBrightnessContrast(brightness, constest);
}

void MainWindow::on_contrast_slid_valueChanged(int value) // This function is used to send contrast value to server
{
    double constest = ui->contrast_slid->value()/contrastdpi;
    double brightness = double(ui->brightness_slid->value());
    cTest->sendBrightnessContrast(brightness, constest);
}

void MainWindow::on_pushButton_2_clicked() //This function is used to send the user selecte image to the server
{
    QString name = "image";

    //Cheak if user has selected an image
    if (!ui->lineEdit->text().isEmpty()){
        //declare a byte array to hold image data
        QByteArray bArray;
        //Load image into qpixmap
        QPixmap pixmap(ui->lineEdit->text());
        //convert image data to buffer
        QBuffer buffer(&bArray);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "bmp");
       //pass the buffer that contains the image data to socket object
        cTest->sendtest(bArray);

    }else{
        qDebug()<<"Image not found!!!";

    }
}
