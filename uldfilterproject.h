#ifndef ULDFILTERPROJECT_H
#define ULDFILTERPROJECT_H

#include <QImage>
#include <QObject>

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>


#include <QSharedPointer>
#include <QQuickItemGrabResult>

#include <QTCPSocket>
#include <QQueue>
#include <QRect>
#include "uldsocket.h"
#include "uldvideodec.h"

/**
 * @brief      This class recieves grabs an image (texture) wrapping a QML item rectangle. And sends it to a file, socket, pipe etc.  
 */
class Uldfilterproject : public QAbstractVideoFilter
{
    Q_OBJECT

    Q_PROPERTY (QString hostName READ hostName WRITE setHostName)
    Q_PROPERTY (qint16 tcpPort READ tcpPort WRITE setTcpPort)
    
/**
 * Shared Pointer to a QQuickItemGrabResult. The QQuickItemGrabResult holds an image with the result of grabbing from a Quick Item.
 */
    struct {
        QString m_hostName;
        quint16 m_port;
    }net;
    
    QThread m_thread;
    UldWorker * m_worker;
    
    
    void connectFilterToHost();
    
    
    
    QByteArray m_imageArray;
    
    
signals:
    void connectionRefusedError(QString hostname, quint16 portNumber);
    void connectionSuccess();    
    void frameDecoded(QByteArray pngChunk, int width, int height);
    
public:
    struct {
        QRect selection;
        bool grab;
    }m_grab;
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
    QVideoFilterRunnable * createFilterRunnable(void) Q_DECL_OVERRIDE;
    Q_INVOKABLE void retrieveImage(QRect r = QRect());
    
    void grabDone(QByteArray);
    QRect grabSelectionRect(){return m_grab.selection;}
    bool isGrabPending(){return m_grab.grab;}

public slots:
    void connectionErrorCheckConnectivity();
    void connectionSuccessSlot();

    
    
    
};
class Uldfilterrunnable: public QVideoFilterRunnable {

public:
    uldvideodec videodecoder;
    QVideoFrame run(QVideoFrame * input, const QVideoSurfaceFormat & sFormat, RunFlags flags) Q_DECL_OVERRIDE;
    void setFilter(Uldfilterproject*f);
private:
    Uldfilterproject * m_f;
    QImage m_img;
    /* Selection rectangle */    
    
    
};


#endif // ULDFILTERPROJECT_H
