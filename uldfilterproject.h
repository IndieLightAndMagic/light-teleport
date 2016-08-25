#ifndef ULDFILTERPROJECT_H
#define ULDFILTERPROJECT_H

#include <QImage>
#include <QQuickItem>
#include <QSharedPointer>
#include <QQuickItemGrabResult>



#include "uld-filter-project_global.h"



class ULDFILTERPROJECTSHARED_EXPORT Uldfilterproject : public QObject
{
    Q_OBJECT

private:
    QSharedPointer<QQuickItemGrabResult> m_grab;
    
public slots:
    void imageRetrieved(void);
    
public:
    Uldfilterproject(QObject * parent = Q_NULLPTR);
    //QVideoFilterRunnable * createFilterRunnable() Q_DECL_OVERRIDE;

    Q_INVOKABLE void retrieveImage(QQuickItem * qItem);

    
};



#endif // ULDFILTERPROJECT_H
