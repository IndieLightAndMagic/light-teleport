#include "uldsocket.h"
#include "uldhelper.h"
#include <QTcpSocket>
#include <QApplication>
#include <QTimer>
UldWorker::UldWorker(QObject* parent):QObject__(parent),
    msRetryTime(2000),
    m_alive(false),
    m_tmr(new QTimer(this))
    
{
    
    
}
void UldWorker::uploadStartSetup(QTcpSocket * s){
    
    /* Error Handling */
    typedef void(QAbstractSocket::*QAbstractSocketError)(QAbstractSocket::SocketError);
    connect(s,static_cast<QAbstractSocketError>(&QAbstractSocket::error),this,&UldWorker::errorNotify);
    
    /* State changed */
    connect(s,&QAbstractSocket::stateChanged,this,&UldWorker::socketStateDisplay);
    
    /* Timer dot */
    connect(m_tmr,SIGNAL(timeout()),this,SLOT(dotDisplay()));
    
    qDebug()<<"socket thread is:"<<s->thread();
    qDebug()<<"timer thread is: "<<m_tmr->thread();
    qDebug()<<"worker thread is:"<<this->thread();
}

void UldWorker::uploadStart_WORKER(QString hostName, quint16 portNumber){
    
    typedef enum {
        WAIT,
        GO
    }WORKERSTATE;
    
    WORKERSTATE workerState = GO;
    
    
    
    
    
    QTcpSocket * s = new QTcpSocket();
    uploadStartSetup(s);
    
    m_alive = true;
    while (m_alive){
        
        QAbstractSocket::SocketState ss;
        ss = s->state();
        if (workerState == WAIT){
            if (!m_tmr->remainingTime()){
                workerState = GO;
                m_tmr->stop();
            } else {
                continue;
            }
            
        }
        
        if (!m_qi.isEmpty()){
            /* Si hay al menos una imagen */
            if (ss == QAbstractSocket::UnconnectedState){
                /* Si no está conectado el socket Conectarse */
                qInfo()<<"Attempting Connection @:"<<hostName<<":"<<portNumber;
                s->connectToHost(hostName,portNumber);
                if (s->waitForConnected() == false){
                    workerState = WAIT;
                    qCritical()<<"Connection Failed... retrying in "<<float(msRetryTime)/1000<<" secs...";
                    m_tmr->stop();
                    m_tmr->start(msRetryTime);
                }
                continue;
                
            } else if (ss == QAbstractSocket::ConnectingState){
                if (!m_tmr->remainingTime()){
                    dotDisplay();
                    m_tmr->start(1000);
                }
                
            } else if (ss == QAbstractSocket::ConnectedState){
                /* Stop pending connection timer */                
                m_tmr->stop();
                
                
                /* Si el socket está conectado, enviar la información */
                /* pop from q and sent Information */
                QImage i = m_qi.dequeue();
                UldHelper::serializeQImageAndSend(s,i);
                

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
    qWarning()<<"Error: "<<error;
    emit connectionErrorCheckConnectivity();
}
void UldWorker::socketStateDisplay(QAbstractSocket::SocketState state){
    qDebug()<<"Socket State: "<<state;
    QApplication::beep();
    
}
void UldWorker::dotDisplay(){
    qDebug()<<".";
}
