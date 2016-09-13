#ifndef ULDFILTERPROJECT_H
#define ULDFILTERPROJECT_H

#include <QImage>
#include <QObject>

#include <QSharedPointer>
#include <QQuickItemGrabResult>

#include <QQueue>

#include "uld-filter-project_global.h"


/**
 * @brief      This class recieves grabs an image (texture) wrapping a QML item rectangle. And sends it to a file, socket, pipe etc.  
 */
class ULDFILTERPROJECTSHARED_EXPORT Uldfilterproject : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString hostName READ hostName WRITE setHostName)
    Q_PROPERTY (int tcpPort READ tcpPort WRITE setTcpPort)
    
/**
 * Shared Pointer to a QQuickItemGrabResult. The QQuickItemGrabResult holds an image with the result of grabbing from a Quick Item.
 */
    
    QSharedPointer<QQuickItemGrabResult> m_pGrab;
    QQueue<QImage> iQ;
    bool m_connected;
    
    QString m_hostName;
    int m_port;
    
    void connectFilterToHost();

public slots:
/**
 * @brief      Do not call this function.
 */
    void imageRetrieved(void);
    
    void setHostName(const QString & hostName);
    QString hostName() const;
    
    void setTcpPort(int tcpPortNumber);
    int tcpPortNumber();
    
public:
	/**<
	 * @brief      Constructor of the Uploaderfilter
	 *
	 * @param      parent  The parent
	 */
    Uldfilterproject(QObject * parent = Q_NULLPTR);
    //QVideoFilterRunnable * createFilterRunnable() Q_DECL_OVERRIDE;

    //Retrieve Image 
    Q_INVOKABLE void retrieveImage(QObject * qItem);

    
    
    
};



#endif // ULDFILTERPROJECT_H
