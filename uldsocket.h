#ifndef ULDSOCKET_H
#define ULDSOCKET_H

#include <QQueue>
#include <QImage>
#include <QThread>
#include <QTcpSocket>
#include <QTimer>
#include "qobject__.h"

class UldWorker:public QObject__{
    Q_OBJECT
public:
    UldWorker(QObject * parent = 0);
    ~UldWorker(){}

signals:
    void uploadFinished();    
    
    
    
public slots:
    void uploadStart_WORKER(QString hostName, quint16 portNumber);
    void imagePush(const QImage & i);
    void setAlive(bool alive);
private slots:
    void errorNotify(QAbstractSocket::SocketError error);
    void socketStateDisplay(QAbstractSocket::SocketState state);
    void dotDisplay();
private:
    QQueue<QImage> m_qi;
    bool m_alive;
    QTimer * m_tmr;
};



#endif // ULDSOCKET_H
