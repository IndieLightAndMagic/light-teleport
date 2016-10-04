#include "uldsocket.h"
#include "uldhelper.h"
#include <QTcpSocket>
#include <QBuffer>
#include <QApplication>
#include <QTimer>

UldWorker::UldWorker(QObject* parent):QObject(parent),
    msRetryTime(2000),
    m_alive(false),
    m_tmr(new QTimer(this))
  
{
    
    
}

void UldWorker::bytesWritten(qint64 bytes){
    
    txStatus.tx_done += bytes;
    float d = txStatus.tx_done;
    float s = txStatus.tx_size;
    
    d = d*100;
    d = d/s;
    
    qInfo()<<"Bytes Written: "<<txStatus.tx_done<<" / "<<txStatus.tx_size<< " ["<<d<<"%]";
    
    m_workerState = RESUME;
}

void UldWorker::uploadStartSetup(QTcpSocket * s){
    
    /* Error Handling */
    typedef void(QAbstractSocket::*QAbstractSocketError)(QAbstractSocket::SocketError);
    connect(s,static_cast<QAbstractSocketError>(&QAbstractSocket::error),this,&UldWorker::errorNotify);
    
    /* State changed */
    connect(s,&QAbstractSocket::stateChanged,this,&UldWorker::socketStateDisplay);
    
    /* Bytes written */
    connect(s,&QIODevice::bytesWritten,this,&UldWorker::bytesWritten);
    
    
    /* Timer dot */
    connect(m_tmr,SIGNAL(timeout()),this,SLOT(dotDisplay()));
    
    qDebug()<<"socket thread is:"<<s->thread();
    qDebug()<<"timer thread is: "<<m_tmr->thread();
    qDebug()<<"worker thread is:"<<this->thread();
}

void UldWorker::uploadStart_WORKER(QString hostName, quint16 portNumber){
    
    
    m_workerState = GO;
    
    
    QTcpSocket * s = new QTcpSocket();
    uploadStartSetup(s);
    
    m_alive = true;
    while (m_alive){
        QByteArray ba;
        QAbstractSocket::SocketState ss;
        ss = s->state();
        if (m_workerState == WAIT){
            if (!m_tmr->remainingTime()){
                m_workerState = GO;
                m_tmr->stop();
            } else {
                continue;
            }
            
        }
        if (m_workerState == UPLOADING){
            /* It's uploading */
            continue;
        }
        if (m_workerState == RESUME){
            
            /* Upload finished */
            /* Check how much info was uploaded, tx_done is the size of the info already sent, tx_size the actual size of the info (bytes)*/
            if (txStatus.tx_done < txStatus.tx_size){
                m_workerState = UPLOADING;
                UldHelper::send(s,ba,txStatus.tx_size,txStatus.tx_done);
                continue;
            }else {
                /* Clear the information */
                ba.clear();
                m_workerState = GO;
            }
        }
        if (m_workerState == GO){
            if (!m_qi.isEmpty()){
                /* Si hay al menos una imagen */
                if (ss == QAbstractSocket::UnconnectedState){
                    /* Si no está conectado el socket Conectarse */
                    qInfo()<<"Attempting Connection @:"<<hostName<<":"<<portNumber;
                    s->connectToHost(hostName,portNumber);
                    if (s->waitForConnected() == false){
                        m_workerState = WAIT;
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
                    /* pop from q */
                    ba = m_qi.dequeue();
                    //ba.clear();
                    /* Add to the byte array additional information */
                    
                    /* Send some pre-data */
                    QJsonObject o = UldHelper::macAndTimeStampJson(s);
                    qDebug()<<"Image Size: "<<ba.size();
                    UldHelper::jsonize(ba,o);
                    qDebug()<<"With Serialized Info:"<<ba.size();
                    
                    
                    /* Configure the status */
                    txStatus.tx_size = ba.size();
                    txStatus.tx_done = 0;
                    
                    /* We Seng and enter UPLOADING state */
                    m_workerState = UPLOADING;
                    UldHelper::send(s,ba,txStatus.tx_size);
                    
                    continue;
                    
                } else {
                    /* Socket is doing whatever I don't care */
                    
                    continue;
                }
                
            } else {
                
                if (s->state() == QAbstractSocket::ConnectedState){
                    /* So there's no workload, free connection */ 
                    qDebug()<<"Closing client";
                    s->disconnectFromHost();
                    s->waitForDisconnected(1000);
                    s->close();
                    continue;
                }
            } 
            
        }
    }
    
    s->disconnectFromHost();
    delete s;
    
}
void UldWorker::imagePush(QImage i){
    
    if (i.isNull()){
        qDebug()<<"Null QImage";
        return;
    }
    /* Sometime in the near future set a QJson Object Along with PNG file */
    //i=i.scaled(256,256);    
    /* Convert image to buffered RAM file */
    QByteArray pngChunk;
    QBuffer pngChunkBuffer(&pngChunk);
    pngChunkBuffer.open(QIODevice::WriteOnly);
    qDebug()<<"PUSH";
    return;
    i.save(&pngChunkBuffer,"PNG");
    m_qi.enqueue(pngChunk);
    qDebug()<<"QUEUE";
    /*i = QImage (QSize(256,256),QImage::Format_ARGB32);
    i.fill(0xff00a413);*/
    
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
