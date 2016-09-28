#include "uldfilterproject.h"
#include <QDebug>
#include <QQuickItem>


Uldfilterproject::Uldfilterproject(QObject* parent) : 
    QObject__(parent),
    m_pGrab(nullptr),
    net({"192.168.1.5",8989}),
    m_worker(new UldWorker())
{
    
    /* Change worker threads */
    m_worker->moveToThread(&m_thread);

    /* Now Connect signals, with a queue: why? this object and m_worker live in different threads now :) */
    connect(this,&Uldfilterproject::awakeWorker,m_worker,&UldWorker::uploadStart_WORKER,Qt::QueuedConnection);
    
    /*Start the thread */    
    m_thread.start();
    
    /* Start */
    emit awakeWorker(net.m_hostName,net.m_port);
    
    
    
    
    
    
}


void Uldfilterproject::imageRetrieved(){
    
    auto grabResult = qobject_cast<QQuickItemGrabResult*>(sender());
    if (grabResult){
        
        qDebug()<<"Grabbed";
        QImage img =  grabResult -> image();
        m_worker->imagePush(img);
        
    } else {
        qDebug()<<"Not Grabbed!";
    }
    
}

void Uldfilterproject::retrieveImage(QObject * qItem){
    
    qDebug()<<"RetrieveImage:"<<qItem;
    auto itm = qobject_cast<QQuickItem*>(qItem);
    m_pGrab = itm  -> grabToImage();
    connect(
                m_pGrab.data(),
                &QQuickItemGrabResult::ready,
                this,
                &Uldfilterproject::imageRetrieved
                );
    
}

void Uldfilterproject::setHostName(const QString & hostName){
    
    net.m_hostName = hostName;
    
}
QString Uldfilterproject::hostName()const{
    return net.m_hostName;
}

void Uldfilterproject::setTcpPort(quint16 tcpPortNumber){
    
    net.m_port = tcpPortNumber;
    
}

int Uldfilterproject::tcpPort(){
    
    return net.m_port;
    
}

void Uldfilterproject::connectFilterToHost(){
    
    
    qDebug()<<"Connecting Filter to:"<<net.m_hostName<<"@"<<net.m_port;
    
    
    
}


