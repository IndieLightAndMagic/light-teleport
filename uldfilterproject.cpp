#include "uldfilterproject.h"
#include <QString>
#include <QDebug>



Uldfilterproject::Uldfilterproject(QObject* parent) : QAbstractVideoFilter(parent)
{
    qDebug() << "Hello World" ;
}

QVideoFilterRunnable * Uldfilterproject::createFilterRunnable(){
    return new UldFilterWorker(this);
}



UldFilterWorker::UldFilterWorker(Uldfilterproject * filter) : 
    m_stop(false),
    m_filter(filter)
{
    qDebug("Filter 0x%lx: ",(unsigned long)m_filter);
    
}
void UldFilterWorker::shutDownWorker(){

    m_stop = true;
    m_filter -> setActive(false);
    emit m_filter -> finished(Q_NULLPTR);
    
}
void UldFilterWorker::retrieveFrame(QVideoFrame * input, QVideoFrame::PixelFormat pFmt){
    
    
    QImage::Format qImgPFmt = QVideoFrame::imageFormatFromPixelFormat(pFmt);
    qDebug() <<" QVideoSurfaceFormat Pixel Format: " << pFmt<< "QImage Format: "<<qImgPFmt;
    qDebug() <<" W:"<<input->width()<<"x H:"<<input->height();
    qDebug() <<" Bytes Per Line: " << input->bytesPerLine();
    qDebug() <<" Mapped Size: " << input->mappedBytes();
    qDebug() <<" Plane Count: " << input->planeCount();
    
    uchar * pPlaneNow = 0x00;
    uchar * pPlaneBefore = 0x00;
    for (int index = 0; index < input->planeCount(); index ++){
        
        ulong addD = 0;
        
        pPlaneNow = input -> bits(index);
        
        if (index==0)   addD = 0;
        else            { addD = (ulong) pPlaneNow ; addD -= (ulong) pPlaneBefore; }
        pPlaneBefore = pPlaneNow;

#define HEXVALUE(v) QString("0x")+QString("%1").arg((ulong)v,0,16)
        qDebug() << "Plane: " << index << "@: " << HEXVALUE(pPlaneNow) << "Size : " << addD;
#undef HEXVALUE    
    }
    
    m_frame = new QImage(
                input->bits(), 
                input -> width(),
                input -> height(),
                input -> bytesPerLine(),
                qImgPFmt);
    
}

QVideoFrame UldFilterWorker::run(QVideoFrame * input, const QVideoSurfaceFormat & sFmt, RunFlags rFlags){

    if (!m_stop){
        QVideoFrame::PixelFormat pFmt = sFmt.pixelFormat();
        bool mapResult = input->map(QAbstractVideoBuffer::ReadOnly);
        if (!mapResult || !(input->isMapped())){
            qCritical("Couldn't map Video Frame. Shutting Filter Down");
            shutDownWorker();
            return *input;
        } 
        /* Get Frame */
        qDebug("Video Frame Mapped");
        qDebug() <<" QVideoSurfaceFormat Pixel Format: " << pFmt << " " << rFlags;
        qDebug() <<" QVideoSurfaceFormat Pixel Format: " << pFmt << " " << rFlags;
        retrieveFrame(input,pFmt);
        qDebug() <<" QImage Pixel Format: " << m_frame -> format();
        qDebug() <<" Sutting Down Filter";
        shutDownWorker();
    }
    
    return *input;
}
