#ifndef ULDFILTERPROJECT_H
#define ULDFILTERPROJECT_H

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>


#include "uld-filter-project_global.h"

class UldFilterWorker: public QVideoFilterRunnable
{
    
private:
    bool m_run;

public:
    UldFilterWorker(){ m_run = false; }
    QVideoFrame run(QVideoFrame * input, const QVideoSurfaceFormat & sFmt, RunFlags rFlags);    
};

class ULDFILTERPROJECTSHARED_EXPORT Uldfilterproject : public QAbstractVideoFilter
{
    
public:
    
    Uldfilterproject(QObject * parent = Q_NULLPTR);
    UldFilterWorker * createFilterRunnable();

};

#endif // ULDFILTERPROJECT_H
