#include "uldfilterproject.h"
#include <QString>
#include <QDebug>



Uldfilterproject::Uldfilterproject(QObject* parent) : QObject(parent)
{
    qDebug() << "Hello World" ;
}
void Uldfilterproject::imageRetrieved(){
    
    qDebug()<<"Signal Arrived! ";
    auto grabResult = qobject_cast<QQuickItemGrabResult*>(sender());
    if (grabResult){
        qDebug()<<"Image Grabbed!";
    } else {
        qDebug()<<"Not Grabbed!";
    }
    
}

void Uldfilterproject::retrieveImage(QQuickItem * qItem){
    
    qDebug()<<qItem;
    QSharedPointer<const QQuickItemGrabResult> m_grab = qItem -> grabToImage();
    qDebug()<<m_grab; 
    connect(
                m_grab.data(),
                SIGNAL(ready()),
                this,
                SLOT(imageRetrieved())
                );
    
}




/*QVideoFilterRunnable * Uldfilterproject::createFilterRunnable(){
    return new UldFilterWorker(this);
}*/


