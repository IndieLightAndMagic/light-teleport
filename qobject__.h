#ifndef QOBJECTxxxH
#define QOBJECTxxxH

#include <QObject>
#include <QDebug>
class QObject__ : public QObject
{
    Q_OBJECT
public:
    QObject__(QObject*parent=0):QObject(parent) {
        qDebug()<<metaObject()->className();
        qDebug()<<"Living@: "<<thread();
        
        
    }
    virtual ~QObject__() {}
};



#endif // QOBJECTxxxH
