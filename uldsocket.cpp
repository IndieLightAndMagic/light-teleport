#include "uldsocket.h"
#include "uldhelper.h"
#include <QTcpSocket>
#include <QBuffer>
#include <QApplication>
#include <QTimer>

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
    
    qInfo()<<"Bytes Written: "<<txStatus.tx_done<<" / "<<txStatus.tx_size<< " ["<<d<<"%]";
    m_workerState = RESUME;

}

void UldWorker::uploadStartSetup(){
    
    /* Error Handling */
    typedef void(QAbstractSocket::*QAbstractSocketError)(QAbstractSocket::SocketError);
    ch.errorNotify = connect(m_s,static_cast<QAbstractSocketError>(&QAbstractSocket::error),this,&UldWorker::errorNotify);
    
    /* State changed */
    ch.stateNotitfy = connect(m_s,&QAbstractSocket::stateChanged,this,&UldWorker::socketStateDisplay);
    
    /* Bytes written */
    ch.bytesWrittenNotify = connect(m_s,&QIODevice::bytesWritten,this,&UldWorker::bytesWritten);
    
    /* Disconnected Sockets */
    ch.disconnectedNotify = connect(m_s,&QAbstractSocket::disconnected,this,&UldWorker::disconnected);
}
void UldWorker::uploadFinished(){
    
    disconnect(ch.errorNotify);
    disconnect(ch.stateNotitfy);
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
                    m_tmr->stop();
                    m_tmr->start(msRetryTime);
                }
                continue;
                        
            } else if(ss == QAbstractSocket::ConnectingState){
                
                if (!m_tmr->remainingTime()){
                    dotDisplay();
                    m_tmr->start(1000);
                }
                
            } else if (ss == QAbstractSocket::ConnectedState){
                
                m_tmr->stop();
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
    
    /* Encode Image into PNG */
    QByteArray pngChunk;
    QBuffer pngChunkBuffer(&pngChunk);
    pngChunkBuffer.open(QIODevice::WriteOnly);
    img.save(&pngChunkBuffer,"PNG");
#ifndef ANDROID
    img.save("test.png","PNG");
#endif    
    /* Get PNGCHUNK and Encode it for Autoformax Format */
    
    //1. Get Json Object Header
    QTcpSocket s;
    QJsonObject o = UldHelper::macAndTimeStampJson(&s);
    qDebug()<<"Image Size: "<<pngChunk.size();
    
    //2. Set JsonObject Header in top of PNGchunk. 
    UldHelper::jsonize(pngChunk,o);
    qDebug()<<"With Serialized Info:"<<pngChunk.size();
    
    //3. Setup transmission.
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
    emit connectionErrorCheckConnectivity();
}

void UldWorker::socketStateDisplay(QAbstractSocket::SocketState state){
    qDebug()<<"Socket State: "<<state;
    QApplication::beep();
    
}

void UldWorker::dotDisplay(){
    qDebug()<<".";
}
