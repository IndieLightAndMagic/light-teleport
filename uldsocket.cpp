#include "uldsocket.h"
#include <QTcpSocket>
#include <QApplication>
#include <QTimer>
UldWorker::UldWorker(QObject* parent):QObject__(parent),
    m_alive(false),
    m_tmr(new QTimer(this))
    
{
    
    
}

void UldWorker::uploadStart_WORKER(QString hostName, quint16 portNumber){
    
    qDebug()<<"Connection Manager Thread";
    m_alive = true;
    
    QTcpSocket * s = new QTcpSocket();
    
    /* Error Handling */
    typedef void(QAbstractSocket::*QAbstractSocketError)(QAbstractSocket::SocketError);
    connect(s,static_cast<QAbstractSocketError>(&QAbstractSocket::error),this,&UldWorker::errorNotify);
    
    /* State changed */
    connect(s,&QAbstractSocket::stateChanged,this,&UldWorker::socketStateDisplay);
    
    /* Timer dot */
    connect(m_tmr,SIGNAL(timeout()),this,SLOT(dotDisplay()));
    m_tmr->start(10000);
    
    qDebug()<<"t thread is:     "<<m_tmr->thread();
    qDebug()<<"this thread is:  "<<this->thread();
    while (m_alive){
        QAbstractSocket::SocketState ss;
        ss = s->state();
        if (!m_qi.isEmpty()){
            /* Si hay al menos una imagen */
            if (ss == QAbstractSocket::UnconnectedState){
                /* Si no está conectado el socket Conectarse */
                //qDebug()<<"Attempting Connection";
                s->connectToHost(hostName,portNumber);
                //s->waitForConnected();
                //t->setInterval(1000);
                //t->setSingleShot(false);
                qDebug()<<m_tmr->remainingTime();
                continue;
                
            } else if (ss == QAbstractSocket::ConnectingState){
            } else if (ss == QAbstractSocket::ConnectedState){
                /* Stop pending connection timer */                
                m_tmr->stop();
                
                
                /* Si el socket está conectado, enviar la información */
                /* pop from q and sent Information */
                QImage i = m_qi.dequeue();
                continue;
            } else {
                /* Socket is doing whatever I don't care */
                continue;
            }
            
        } else {
            
            if (s->state() == QAbstractSocket::ConnectedState){
                /* So there's no workload, free connection */ 
                s->disconnectFromHost();    
                continue;
            }
        }
    }
    
    s->disconnectFromHost();
    delete s;

}
void UldWorker::imagePush(const QImage &i){
    
    m_qi.enqueue(i);
    
}
void UldWorker::setAlive(bool alive){
    m_alive = alive;
}

void UldWorker::errorNotify(QAbstractSocket::SocketError error){
    qDebug()<<"Goal!!!!";
    qDebug()<<"Error: "<<error;
}
void UldWorker::socketStateDisplay(QAbstractSocket::SocketState state){
    qDebug()<<"Socket State: "<<state;
    QApplication::beep();
    
}
void UldWorker::dotDisplay(){
    qDebug()<<".";
}
