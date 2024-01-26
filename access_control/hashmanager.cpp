#include "hashmanager.h"
HashManager::HashManager(){};
HashManager::HashManager(QCryptographicHash::Algorithm _hashFunction)
{
    hashFunction=_hashFunction;
}

QString HashManager::hashString(const QString& str) const{
    QByteArray str_byte_array = str.toUtf8();
    QByteArray hash_byte_array = QCryptographicHash::hash(str_byte_array, hashFunction);
    return QString(hash_byte_array.toHex());
}
