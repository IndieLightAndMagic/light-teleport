#include "uldfilterproject.h"
#include <QString>
#include <QDebug>
#include <QQuickItem>



Uldfilterproject::Uldfilterproject(QObject* parent) : QObject(parent),m_pGrab(nullptr)
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




/*QVideoFilterRunnable * Uldfilterproject::createFilterRunnable(){
    return new UldFilterWorker(this);
}*/


