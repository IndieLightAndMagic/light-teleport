#ifndef ULDFILTERPROJECT_H
#define ULDFILTERPROJECT_H

#include <QImage>
#include <QObject>

#include <QSharedPointer>
#include <QQuickItemGrabResult>

#include <QTCPSocket>
#include <QQueue>
#include <QRect>
#include "uldsocket.h"

/**
 * @brief      This class recieves grabs an image (texture) wrapping a QML item rectangle. And sends it to a file, socket, pipe etc.  
 */
class Uldfilterproject : public QObject__
{
    Q_OBJECT

    Q_PROPERTY (QString hostName READ hostName WRITE setHostName)
    Q_PROPERTY (int tcpPort READ tcpPort WRITE setTcpPort)
    
/**
 * Shared Pointer to a QQuickItemGrabResult. The QQuickItemGrabResult holds an image with the result of grabbing from a Quick Item.
 */
    
    QSharedPointer<QQuickItemGrabResult> m_pGrab;
    
    struct {
        QString m_hostName;
        quint16 m_port;
    }net;
    
    QThread m_thread;
    UldWorker * m_worker;
    
    /* Selection rectangle */    
    QRect m_rSelection;
    
    
    void connectFilterToHost();
    
signals:
    void awakeWorker(QString hostname, quint16 portNumber);    
    
private slots:
/**
 * @brief      Do not call this function.
 */
    void imageRetrieved(void);

    
public:
    void setHostName(const QString & hostName);
    QString hostName() const;
    
    void setTcpPort(quint16 tcpPortNumber);
    int tcpPort();
    
    
	/**<
	 * @brief      Constructor of the Uploaderfilter
	 *
	 * @param      parent  The parent
	 */
    Uldfilterproject(QObject * parent = Q_NULLPTR);
    
    //Retrieve Image 
    Q_INVOKABLE bool retrieveSubImage(QObject * qItem, int initial_x, int initial_y, int final_x, int final_y);
    Q_INVOKABLE bool retrieveImage(QObject * qItem);
    
    
    
    
};



#endif // ULDFILTERPROJECT_H
