#ifndef ULDFILTERPROJECT_H
#define ULDFILTERPROJECT_H

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>


#include "uld-filter-project_global.h"



class ULDFILTERPROJECTSHARED_EXPORT Uldfilterproject : public QAbstractVideoFilter
{
    
public:
    
    Uldfilterproject(QObject * parent = Q_NULLPTR);
    QVideoFilterRunnable * createFilterRunnable() Q_DECL_OVERRIDE;

signals:
    void finished(QObject * result);
    
};

class UldFilterWorker: public QVideoFilterRunnable
{
    
private:
    bool m_run;
    Uldfilterproject * m_filter;
    
public:
    UldFilterWorker(Uldfilterproject * filter);
    QVideoFrame run(QVideoFrame * input, const QVideoSurfaceFormat & sFmt, RunFlags rFlags);    
};


#endif // ULDFILTERPROJECT_H
