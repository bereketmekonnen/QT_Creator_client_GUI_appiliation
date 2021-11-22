#include "socket.h"
#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QBuffer>

socket::socket(QObject *parent) : QObject(parent)//This function is used to establish a tcp connection with the server/FPGA board
{
    //Create a QTCP socket
    sockett = new QTcpSocket(this);
    //connect the socket to server by using the server's IP address and a specific
    sockett->connectToHost("192.168.1.233", 8080);
    //if the socket successfully connects to the server print a success message
    if(sockett->waitForConnected(3000)){
        qDebug()<<"connected to host";
        //socket->write("cool");
    }
    //display an error message
    else{
        qDebug()<<"couldn't connect to host";
    }
}

void socket::sendBrightnessContrast(double brightness, double contrast){ // This function is used to send the brightness and contrast values to the server
//declare a string that is used in the datagrame
    QString str0 = "b/c";
    QString strb = QString::number(brightness);
    QString strc = QString::number(contrast);
    int head1 = 7;
    //qDebug() <<"image size:" << str1;
    str0.resize(headsize0,' ');
    strb.resize(head1,' ');
    strc.resize(head1,' ');
//declare a bytearray that will carry the code, which will be used to determin what kind of data is being sent by the server
    QByteArray code = str0.toLocal8Bit();
//convert brightness and contrast value into byte array
    QByteArray bytesBrightness = strb.toLocal8Bit();
    QByteArray bytesContrast= strc.toLocal8Bit();
    //concatenate code, brightness value and contrast value and send it to the server by writeing to to the socket
    sockett->write(code+bytesBrightness+bytesContrast);
    sockett->flush();
    sockett->waitForBytesWritten(200);
}



void socket::sendtest(QByteArray image){ //This function is used to send image data to the server
//declare datagram code
    QString str0 = "image";
    //obtain the size of image data
    QString str1 = QString::number(image.size());

    qDebug() <<"image size:" << str1;
    //resize the strings that hold the code and image size
    str0.resize(headsize0,' ');
    str1.resize(headsize1,' ');
    //convert the code and image size to  local 8-bit representation of the string as a QByteArray
    QByteArray code = str0.toLocal8Bit();
    QByteArray size = str1.toLocal8Bit();
    //send the code, image size and image data to server
    sockett->write(code+size+image);
    sockett->flush();
    sockett->waitForBytesWritten(200);

}
