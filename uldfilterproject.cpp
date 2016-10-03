#include "uldfilterproject.h"
#include <QDebug>
#include <QQuickItem>


Uldfilterproject::Uldfilterproject(QObject* parent) : 
    QAbstractVideoFilter (parent),
    net({"192.168.1.5",8989}),
    m_worker(new UldWorker()),
    m_grab({QRect(),false})
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




void Uldfilterproject::retrieveImage(QRect r){
    
    m_grab.grab = true;
    m_grab.selection = r;
    
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

QVideoFilterRunnable * Uldfilterproject::createFilterRunnable(){
        
    Uldfilterrunnable * fr = new Uldfilterrunnable;
    fr->setFilter(this);
    return fr;
    

}
void Uldfilterrunnable::setFilter(Uldfilterproject*f){
    
    m_f=f;
    
}
QVideoFrame Uldfilterrunnable::run(QVideoFrame * input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags){
    
    Q_UNUSED(surfaceFormat);
    Q_UNUSED(flags);
    
    if (m_f->isGrabPending()){
        QRect selectionRectangle = m_f -> grabSelectionRect();
        input->map(QAbstractVideoBuffer::ReadOnly);
        QImage vimgtotal = videodecoder.toARGB32(input);
        input->unmap();
        QImage vimgselection = selectionRectangle.isNull() ? vimgtotal : vimgtotal.copy(selectionRectangle);
        m_f->grabDone(vimgselection);
    }
    
    return *input;
    
}
    
