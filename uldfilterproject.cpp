#include "uldfilterproject.h"

#include <QDebug>



Uldfilterproject::Uldfilterproject(QObject* parent) : QAbstractVideoFilter(parent)
{
    qDebug() << "Hello World" ;
}

UldFilterWorker * Uldfilterproject::createFilterRunnable(){
    return new UldFilterWorker;
}

QVideoFrame UldFilterWorker::run(QVideoFrame * input, const QVideoSurfaceFormat & sFmt, RunFlags rFlags){
    if (!m_run){
        QVideoFrame::PixelFormat pFmt = sFmt.pixelFormat();
        qDebug() <<"Hello run " << pFmt << " " << rFlags;
        m_run = true;
    }
    return *input;
}
