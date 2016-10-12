#include "uldsocket.h"
#include "uldhelper.h"
#include <QTcpSocket>
#include <QBuffer>
#include <QApplication>
#include <QTimer>
#include <QMatrix>

UldWorker::UldWorker(QObject* parent):QObject(parent),
    msRetryTime(2000),
    m_alive(false),
    m_tmr(new QTimer(this)), 
    m_s(Q_NULLPTR),
    hostName("192.168.1.1"),
    portNumber()
  
{
}
void UldWorker::disconnected(){
    qDebug()<<"SOCKET WAS DISCONNECTED";
    m_deadSocket = m_s;
    m_s->close();
    m_deadSocket->deleteLater();
}

void UldWorker::setHostPortNumber(QString hostName, quint16 portNumber){

    this->hostName = hostName;
    this->portNumber = portNumber;    

}

void UldWorker::bytesWritten(qint64 bytes){
    
    txStatus.tx_done += bytes;
    float d = txStatus.tx_done;
    float s = txStatus.tx_size;
    
    d = d*100;
    d = d/s;
#ifndef ANDROID    
    qInfo()<<"Bytes Written: "<<txStatus.tx_done<<" / "<<txStatus.tx_size<< " ["<<d<<"%]";
#endif    
    m_workerState = RESUME;

}

void UldWorker::uploadStartSetup(){
    
    /* Error Handling */
    typedef void(QAbstractSocket::*QAbstractSocketError)(QAbstractSocket::SocketError);
    ch.errorNotify = connect(m_s,static_cast<QAbstractSocketError>(&QAbstractSocket::error),this,&UldWorker::errorNotify);
    
    /* State changed */
    //ch.stateNotitfy = connect(m_s,&QAbstractSocket::stateChanged,this,&UldWorker::socketStateDisplay);
    
    /* Bytes written */
    ch.bytesWrittenNotify = connect(m_s,&QIODevice::bytesWritten,this,&UldWorker::bytesWritten);
    
    /* Disconnected Sockets */
    ch.disconnectedNotify = connect(m_s,&QAbstractSocket::disconnected,this,&UldWorker::disconnected);
}
void UldWorker::uploadFinished(){
    
    disconnect(ch.errorNotify);
    //disconnect(ch.stateNotitfy);
    disconnect(ch.bytesWrittenNotify);
    
}
void UldWorker::upload(QByteArray ba){
    
    /* Create a new Socket and its connections*/
    m_s = new QTcpSocket;
    uploadStartSetup();
    
    
    /* Initialize the Transmission State */
    m_workerState = GO;
    qDebug()<<"GO";
    
    
    /* Transmission Machine State Loop */
    while (m_workerState != IDLE){
        if (m_workerState == UPLOADING){
            /* Go somewhere else cause monkey is working */
            QThread::yieldCurrentThread();
            continue;
        }
        /* When waiting */
        else if (m_workerState == WAIT){
            if (!m_tmr->remainingTime()){
                m_workerState = GO;
                qDebug()<<"GO";
                m_tmr->stop();
            } else {
                continue;
            }
        }
        /* When part of the chunk was written */
        else if (m_workerState == RESUME){
            /* Check how much info was uploaded, tx_done is the size of the info already sent, tx_size the actual size of the info (bytes)*/
            if (txStatus.tx_done < txStatus.tx_size){
                /* Uploading */
                m_workerState = UPLOADING;
                UldHelper::send(m_s,ba,txStatus.tx_size,txStatus.tx_done);
                continue;
            }else {
                /* Tx finished */
                ba.clear();
                m_workerState = FINISH;
                qDebug()<<"FINISH";
            }            
        }
        else if (m_workerState == GO){
            /* State */
            QAbstractSocket::SocketState ss = m_s -> state();
            if (ss == QAbstractSocket::UnconnectedState){
                /* Si no está conectado el socket Conectarse */
                qInfo()<<"Attempting Connection @:"<<hostName<<":"<<portNumber;
                m_s -> connectToHost(hostName,portNumber);
                if (m_s -> waitForConnected() == false){
                    m_workerState = WAIT;
                    qDebug()<<"WAIT";
                    qCritical()<<"Connection Failed... retrying in "<<float(msRetryTime)/1000<<" secs...";
                    emit connectionErrorCheckConnectivity();
                    m_tmr->stop();
                    m_tmr->start(msRetryTime);
                } else {
                    emit connectionSuccess();
                }
                continue;
                        
            } else if(ss == QAbstractSocket::ConnectingState){
                
                if (!m_tmr->remainingTime()){
                    dotDisplay();
                    m_tmr->start(1000);
                }
                
            } else if (ss == QAbstractSocket::ConnectedState){
                
                m_tmr->stop();
                
                //1. Get Json Object Header
                QJsonObject o = UldHelper::macAndTimeStampJson(m_s);
                qDebug()<<"Image Size: "<<ba.size();
                
                //2. Set JsonObject Header in top of PNGchunk. 
                UldHelper::jsonize(ba,o);
                qDebug()<<"With Serialized Info:"<<ba.size();
                txStatus.tx_size = ba.size();
                txStatus.tx_done = 0;
                
                m_workerState = UPLOADING;
                qDebug()<<"UPLOADING";
                UldHelper::send(m_s,ba,txStatus.tx_size);            
                
            }                 
            
        } else if (m_workerState == FINISH){
            QAbstractSocket::SocketState ss = m_s -> state();
            if (ss == QAbstractSocket::ConnectedState){
                qDebug()<<"Ok transmission FINISHED";
                m_s->disconnectFromHost();
                m_s->waitForDisconnected(1000);
            }
            m_workerState = IDLE;
            qDebug()<<"IDLE SOCKET";
            
        }
    }
    
}

void UldWorker::pushImage(QByteArray imageChunk, int width, int height){

    /* image chunk */
    uchar * baidata = reinterpret_cast<uchar*>(imageChunk.data());
    
    /* Create QImage */
    QImage img = QImage(baidata, width, height, QImage::Format_ARGB32);
#ifdef ANDROID
    QMatrix rotationMatrix;
    rotationMatrix.rotate(90);
    img = img.transformed(rotationMatrix);
#endif
    /* Encode Image into PNG */
    QByteArray pngChunk;
    QBuffer pngChunkBuffer(&pngChunk);
    pngChunkBuffer.open(QIODevice::WriteOnly);
    //img = img.scaled(img.width()>>7, img.height()>>7);
    img.save(&pngChunkBuffer,"PNG");
#ifndef ANDROID
    img.save("test.png","PNG");
#endif    
    
    /*. Setup transmission.*/
    upload(pngChunk);
    
    
    
}
void UldWorker::testSlot(){
    qDebug()<<"slot";
}

void UldWorker::setAlive(bool alive){
    m_alive = alive;
}

void UldWorker::errorNotify(QAbstractSocket::SocketError error){
    qWarning()<<"Error: "<<error;
    if (error == QAbstractSocket::ConnectionRefusedError)
        emit connectionErrorCheckConnectivity();
}

void UldWorker::socketStateDisplay(QAbstractSocket::SocketState state){
    qDebug()<<"Socket State: "<<state;
    QApplication::beep();
    
}

void UldWorker::dotDisplay(){
    qDebug()<<".";
}
