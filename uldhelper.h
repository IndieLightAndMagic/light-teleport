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
    /*!
     * \brief send is a function to send a data buffer (bytearray) through a network connection.
     * \param as The network connection. 
     * \param pngChunk The bytearray representing the buffered data
     * \param maxSize Tx Size.
     * \param offsetIndex Bytearray Buffer Start Index.
     */
    static void send(QAbstractSocket * as, QByteArray &pngChunk,qint64 maxSize, qint64 offsetIndex=0);
    
    /*!
     * \brief jsonize return a json object's bytearray string representation. The json string is set on top (at the beginning) of bytearray ba. Also the size in bytes of the json representation is set at the beggninig, using 4 bytes.\n
     *   So in the beginning we have: ba = [ Data in Buffer], after the function ba = [jsonRepSize:jsonRep:Data in buffer].   
     * \param ba the initial bytearray
     * \param jo The json object.
     */
    static void jsonize(QByteArray & ba,QJsonObject & jo);
    static void serialize(QByteArray &ba);

    static QByteArray swapEndianness(int);
    static QJsonObject macAndTimeStampJson(QAbstractSocket* as);

};

#endif // ULDHELPER_H
