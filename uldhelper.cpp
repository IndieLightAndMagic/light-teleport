#include "uldhelper.h"
#include <QDebug>
#include <QDateTime>
UldHelper::UldHelper()
{
    
}

QByteArray UldHelper::formatMessage(char * buff){
    
    
    
    QString qsbuff = QString(buff)+QDateTime::currentDateTime().toString();
    int bsz =  qsbuff.size();
    int * pbsz = &bsz;
    quint16 zero = 0;
    char * prebuff  = reinterpret_cast<char*>(pbsz);
    
    char swap = prebuff[1];
    
    prebuff[1] = prebuff[0];
    prebuff[0] = swap;
    
    
    
    QByteArray ba(reinterpret_cast<const char*>(&zero),2);
    ba+=QByteArray(prebuff,2);
    ba += qsbuff;
    qDebug()<<ba<<ba.size();
    return ba;
    
}
void UldHelper::serializeQImageAndSend(QAbstractSocket * as,const QImage &i){
    
    
    
    char msg[] = "Hello, I am not the client";
    QByteArray bmsg = formatMessage(msg);
    qint64 bWritten = as->write(bmsg);
    
    qInfo()<<"Bytes Written:"<<bWritten;
    qInfo()<<as->waitForBytesWritten(bmsg.size());
    
    
}
