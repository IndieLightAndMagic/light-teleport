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
    int msRetryTime;

signals:
    void uploadFinished();    
    void connectionErrorCheckConnectivity();
    
public slots:
    void uploadStart_WORKER(QString hostName, quint16 portNumber);
    void imagePush(QImage & i);
    void setAlive(bool alive);

private slots:
    void bytesWritten(qint64 bytes);
    void errorNotify(QAbstractSocket::SocketError error);
    void socketStateDisplay(QAbstractSocket::SocketState state);
    void dotDisplay();

private:
    void uploadStartSetup(QTcpSocket * s);
    QQueue<QByteArray> m_qi;
    bool m_alive;
    QTimer * m_tmr;
    
    typedef enum {
        WAIT,
        GO,
        UPLOADING,
        RESUME,
    }WORKERSTATE;
    
    WORKERSTATE m_workerState;
    
    
    struct {
        qint64 tx_size;
        qint64 tx_done;
    }txStatus;
    
};



#endif // ULDSOCKET_H
