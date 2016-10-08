
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

void UldHelper::jsonize(QByteArray & ba, QJsonObject & jo){
    
    
    QJsonDocument jod(jo);
    QByteArray jodba = jod.toJson(QJsonDocument::Compact);
    qDebug()<<"Json size is: "<<jodba.size();
    serialize(jodba);
    ba = jodba + ba;
    serialize(ba);
        
}

void UldHelper::serialize(QByteArray & pngChunk){

    /* Save a PNG file on RAM and upload it */
    int pngChunkSize = pngChunk.size();
    pngChunk = swapEndianness(pngChunkSize)+ pngChunk;
    
}

void UldHelper::send(QAbstractSocket * as, QByteArray &pngChunk, qint64 maxSize, qint64 offsetIndex){
    
    char * data = pngChunk.data();
    qint64 stripSize = maxSize-offsetIndex;
    as->write(&(data[offsetIndex]),stripSize);
    
    //as->flush();
    
    /* This did not work */
    bool writeOperationResult=as->waitForBytesWritten(-1);
    if (!writeOperationResult) qCritical()<<"writeOperation NOT Ok";
    
}
QJsonObject UldHelper::macAndTimeStampJson(QAbstractSocket * as){
    
    QJsonObject o;
    
    //QList<QNetworkInterface> il = QNetworkInterface::allInterfaces();
    QList<QHostAddress> hl = QNetworkInterface::allAddresses();
    
    QHostAddress localAddress = as -> localAddress();
    
    QString hadd;
    int index = 0; 
    bool found = false;
#ifndef ANDROID    
    for (QList<QHostAddress>::iterator host = hl.begin(); host!= hl.end(); host++){
        if (found)continue;
        if (host[0] == localAddress){
            found = true;
            QNetworkInterface qni = QNetworkInterface::interfaceFromIndex(index);
            hadd = qni.hardwareAddress();
        }
        index++;
    
    }
    hadd = found ? hadd : QNetworkInterface::interfaceFromIndex(0).hardwareAddress();
#else
    hadd = "AN:DR:OI:DB:UI:LD";
#endif
    QStringList octets = hadd.split(":");
    hadd = octets.join("");
    o["hwAdd"] = hadd;
    QString dtString = QDateTime::currentDateTimeUtc().toString("yy.MM.dd.HH.mm.ss.zzz");
    QStringList dtStringList = dtString.split(".");
    dtString = dtStringList.join("");
    o["tsUtc"] = dtString;
    qDebug()<<found<<"->"<<o;
    
    return o;
    
    
}
