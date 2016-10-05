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
    m_s(&_m_s),
    hostName("192.168.1.5"),
    portNumber()
  
{
    /* Connect */
    connect(this,SIGNAL(dataOnTheQueue()),SLOT(sendData()));
    
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
    
}
void UldWorker::uploadFinished(){
    
    disconnect(ch.errorNotify);
    disconnect(ch.stateNotitfy);
    disconnect(ch.bytesWrittenNotify);
    
}

void UldWorker::pushImage(QByteArray pngChunk){
    
    /* Convert to PNG byte array*/
    qDebug()<<"DATA---->";    
        
    
    
    /* Format PNG */
    QJsonObject o = UldHelper::macAndTimeStampJson(m_s);
    qDebug()<<"Image Size: "<<pngChunk.size();
    UldHelper::jsonize(pngChunk,o);
    qDebug()<<"With Serialized Info:"<<pngChunk.size();
    
    m_qi.enqueue(pngChunk);

    emit dataOnTheQueue();
}   


void UldWorker::sendData(){
    
    qDebug()<<"Sending Data";
    QByteArray ba = m_qi.dequeue();
    
    
    
    qDebug()<<"Data Sent";
    
    
        
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
