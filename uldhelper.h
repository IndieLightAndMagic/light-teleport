#ifndef ULDHELPER_H
#define ULDHELPER_H

#include <QObject>
#include <QAbstractSocket>
class UldHelper
{
public:
    UldHelper();
    static QByteArray formatMessage(char * buff);
    static void freeFormatMessage(char*);
    static void serializeQImageAndSend(QAbstractSocket * as,const QImage &i);
};

#endif // ULDHELPER_H
