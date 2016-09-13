#include "uldfilterproject.h"
#include <QString>
#include <QDebug>
#include <QQuickItem>



Uldfilterproject::Uldfilterproject(QObject* parent) : 
    QObject(parent),
    m_pGrab(nullptr),
    m_connected(false),
    m_port(0)
{
    qDebug() << "Hello World" ;
}
void Uldfilterproject::imageRetrieved(){
    
    qDebug()<<"Signal Arrived! ";
    auto grabResult = qobject_cast<QQuickItemGrabResult*>(sender());
    if (grabResult){
        
        const QImage img =  grabResult -> image();
        qDebug()<<"Image Grabbed: "<<img;
        qDebug()<<"Image Size: "<<img.size();
        grabResult->saveToFile("dummyfile.png");
        
        
        
    } else {
        
        qDebug()<<"Not Grabbed!";
    
    }
    
}

void Uldfilterproject::retrieveImage(QObject * qItem){
    
    qDebug()<<qItem;
    auto itm = qobject_cast<QQuickItem*>(qItem);
    
    m_pGrab = itm  -> grabToImage();
    qDebug()<<m_pGrab; 
    
    connect(
                m_pGrab.data(),
                &QQuickItemGrabResult::ready,
                this,
                &Uldfilterproject::imageRetrieved
                );
    
}

void Uldfilterproject::setHostName(const QString & hostName){
    
    m_hostName = hostName;
    
}
QString Uldfilterproject::hostName()const{
    return m_hostName;
}

void Uldfilterproject::setTcpPort(int tcpPortNumber){
    
    m_port = tcpPortNumber;
    
}

int Uldfilterproject::tcpPortNumber(){
    
    return m_port;
    
}

void Uldfilterproject::connectFilterToHost(){
    
    
    qDebug()<<"Connecting Filter to:"<<m_hostName<<"@"<<m_port;
    
    
    
}

/*QVideoFilterRunnable * Uldfilterproject::createFilterRunnable(){
    return new UldFilterWorker(this);
}*/


