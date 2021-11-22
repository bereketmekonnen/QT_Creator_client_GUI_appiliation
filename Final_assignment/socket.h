#ifndef SOCKET_H
#define SOCKET_H
#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QBuffer>

class socket: public QObject
{
    Q_OBJECT
public:

    explicit socket(QObject *parent = nullptr);
    //socket object member functions
    void sendBrightnessContrast(double brightness, double contrast);
    void sendtest(QByteArray image);

signals:

public slots:

private:
    //socket object member variables
    QTcpSocket *sockett;
    int headsize0=40;
    int headsize1=50;


};

#endif // SOCKET_H
