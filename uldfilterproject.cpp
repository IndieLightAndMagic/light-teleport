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
    connect(this,SIGNAL(frameDecoded(QByteArray)),m_worker,SLOT(pushImage(QByteArray)),Qt::QueuedConnection);
    
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
    m_grab.grab = false;
    
    
    
    
    emit this->frameDecoded(pngChunk); 
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
        QRect selectionRectangle = m_f -> grabSelectionRect();
        input->map(QAbstractVideoBuffer::ReadWrite);
        qDebug()<<input->mappedBytes();
        QImage vimgselection = QImage(QSize(256,256),QImage::Format_ARGB32); //videodecoder.toARGB32(input);
        //vimgselection.fill(0xff340012);
        input->unmap();
        m_img = selectionRectangle.isNull() ? vimgselection : vimgselection.copy(selectionRectangle);
        
        QByteArray pngChunk;
        QBuffer pngChunkBuffer(&pngChunk);
        pngChunkBuffer.open(QIODevice::WriteOnly);
        vimgselection.save(&pngChunkBuffer,"PNG",99);
        
        m_f->grabDone(pngChunk);
        qDebug()<<"Grab Done";
    }
    
    return *input;
    
}
    
