#ifndef ULDHELPER_H
#define ULDHELPER_H

#include <QObject>
#include <QImage>
#include <QAbstractSocket>
class UldHelper
{
public:
    UldHelper();
    static QByteArray formatMessage(char * buff);
    static void freeFormatMessage(char*);
    static void serializeQImageAndSend(QAbstractSocket * as,QImage &i);

    static QByteArray swapEndianness(int);

};

#endif // ULDHELPER_H
