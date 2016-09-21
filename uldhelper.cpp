
#include "uldhelper.h"
#include <QDebug>
#include <QDateTime>
#include <QBuffer>
#include <QAbstractSocket>
#include <QNetworkInterface>
#include <QJsonDocument>
#include <QJsonObject>
UldHelper::UldHelper()
{
    
}

QByteArray UldHelper::swapEndianness(int i){
    
    unsigned char * pi = reinterpret_cast<unsigned char *>(&i);
    unsigned char swapper = pi[3];
    pi[3] = pi[0];
    pi[0] = swapper;
    
    swapper = pi [2];
    pi[2] = pi[1];
    pi[1] = swapper;
    
    return QByteArray(reinterpret_cast<const char*>(pi),4);
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
void UldHelper::jsonize(QByteArray & ba, QJsonObject & jo){
    
    
    QJsonDocument jod(jo);
    QByteArray jodba = jod.toJson(QJsonDocument::Compact);
    serialize(jodba);
    ba = jodba + ba;
        
}

void UldHelper::serialize(QByteArray & pngChunk){

    /* Save a PNG file on RAM and upload it */
    int pngChunkSize = pngChunk.size();
    
    qInfo()<<"Buffered PNG Size:"<<pngChunk.size();
    pngChunk = swapEndianness(pngChunkSize)+ pngChunk;
    
}

void UldHelper::send(QAbstractSocket * as, QByteArray &pngChunk, qint64 maxSize, qint64 offsetIndex){
    
    char * data = pngChunk.data();
    qInfo()<<"OffsetIndex is:"<<offsetIndex;
    
    qint64 stripSize = maxSize-offsetIndex;
    qint64 bWritten = as->write(&(data[offsetIndex]),stripSize);
    
    //as->flush();
    
    /* This did not work */
    qInfo()<<"Write Functions says bytes written:"<<bWritten;
    bool writeOperationResult=as->waitForBytesWritten(-1);
    if (writeOperationResult) qDebug()<<"writeOperation Ok";
    else qCritical()<<"writeOperation NOT Ok";
    
}
QJsonObject UldHelper::macAndTimeStampJson(QAbstractSocket * as){
    
    QJsonObject o;
    //QList<QNetworkInterface> il = QNetworkInterface::allInterfaces();
    QList<QHostAddress> hl = QNetworkInterface::allAddresses();
    
    QHostAddress localAddress = as -> localAddress();
    
    QString hadd;
    int index = 0; 
    for (QList<QHostAddress>::iterator host = hl.begin(); host!= hl.end(); host++){
        if (host[0] == localAddress){
            QNetworkInterface qni = QNetworkInterface::interfaceFromIndex(index);
            hadd = qni.hardwareAddress();
            QStringList octets = hadd.split(":");
            hadd = octets.join("");
        
        }
        index++;
    }
    o["hwAddr"] = hadd;
    QString dtString = QDateTime::currentDateTimeUtc().toString("yy.MM.dd.HH.mm.ss.zzz");
    QStringList dtStringList = dtString.split(".");
    dtString = dtStringList.join("");
    o["tmStmp"] = dtString;
    
    return o;
    
    
}
