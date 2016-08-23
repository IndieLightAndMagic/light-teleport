#include "uldfilterproject.h"

#include <QDebug>



Uldfilterproject::Uldfilterproject(QObject* parent) : QAbstractVideoFilter(parent)
{
    qDebug() << "Hello World" ;
}

QVideoFilterRunnable * Uldfilterproject::createFilterRunnable(){
    return new UldFilterWorker(this);
}



UldFilterWorker::UldFilterWorker(Uldfilterproject * filter) : 
    m_run(false),
    m_filter(filter)
{
    qDebug("Filter 0x%lx: ",(unsigned long)m_filter);
    
}

QVideoFrame UldFilterWorker::run(QVideoFrame * input, const QVideoSurfaceFormat & sFmt, RunFlags rFlags){

    if (!m_run){
        QVideoFrame::PixelFormat pFmt = sFmt.pixelFormat();
        qDebug() <<"Hello run " << pFmt << " " << rFlags;
        m_run = true;
    }
    
    return *input;
}
