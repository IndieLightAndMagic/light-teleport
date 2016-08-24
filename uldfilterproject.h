#ifndef ULDFILTERPROJECT_H
#define ULDFILTERPROJECT_H

#include <QImage>
#include <QQuickItem>
#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>


#include "uld-filter-project_global.h"



class ULDFILTERPROJECTSHARED_EXPORT Uldfilterproject : public QAbstractVideoFilter
{
    Q_OBJECT
    
public:
    Uldfilterproject(QObject * parent = Q_NULLPTR);
    QVideoFilterRunnable * createFilterRunnable() Q_DECL_OVERRIDE;

    Q_INVOKABLE void retrieveImage(int id, const QString & imgURL);

signals:
    void finished(QObject * result);
    
};

class UldFilterWorker: public QVideoFilterRunnable
{
    
    
private:
    bool m_stop;
    Uldfilterproject * m_filter;
    void shutDownWorker();
    
    QImage * m_frame;
    void retrieveFrame(QVideoFrame * iFrame, QVideoFrame::PixelFormat pFmt);
public:
    UldFilterWorker(Uldfilterproject * filter);
    QVideoFrame run(QVideoFrame * input, const QVideoSurfaceFormat & sFmt, RunFlags rFlags);    
};


#endif // ULDFILTERPROJECT_H
