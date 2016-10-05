#include "uldfilterproject.h"
#include <QDebug>
#include <QQuickItem>
#include <QBuffer>

Uldfilterproject::Uldfilterproject(QObject* parent) : 
    QAbstractVideoFilter (parent),
    net({"192.168.1.5",8989}),
    m_worker(new UldWorker()),
    m_grab({QRect(),false})
{
    
    /* Change worker threads */
    m_worker->moveToThread(&m_thread);
    /* Set address */
    m_worker->setHostPortNumber(net.m_hostName,net.m_port);
    
    /* Connect end of decoding to image sending */
    connect(this,SIGNAL(frameDecoded(QByteArray,int,int)),m_worker,SLOT(pushImage(QByteArray,int,int)),Qt::QueuedConnection);
    
    m_thread.start();
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

void Uldfilterproject::grabDone(QByteArray pngChunk){
    Q_UNUSED(pngChunk);
    qDebug()<<"Grabbing Image Chunk";
    m_grab.grab = false;
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
    QByteArray ba;
    if (m_f->isGrabPending()){
        qDebug()<<input->map(QAbstractVideoBuffer::ReadOnly);
	ba = videodecoder.toARGB32ByteArray(input);
        input->unmap();
	emit m_f->frameDecoded(ba,input->width(),input->height());
        qDebug()<<"Grab Done";
	m_f->m_grab.grab = false;
    }
    return *input;
    
}
    
