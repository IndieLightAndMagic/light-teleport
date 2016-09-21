#ifndef ULDHELPER_H
#define ULDHELPER_H

#include <QObject>
#include <QJsonObject>
#include <QAbstractSocket>

#define ULD_HELPER_MAX_CHUNK_SIZE 131768


class UldHelper
{
    
public:
    UldHelper();
    
    static void send(QAbstractSocket * as, QByteArray &pngChunk,qint64 maxSize, qint64 offsetIndex=0);
    
    static void jsonize(QByteArray & ba,QJsonObject & jo);
    static void serialize(QByteArray &ba);

    static QByteArray swapEndianness(int);
    static QJsonObject macAndTimeStampJson(QAbstractSocket* as);

};

#endif // ULDHELPER_H
