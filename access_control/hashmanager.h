#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include "QtCore"

class HashManager
{
public:
    HashManager();
    HashManager(QCryptographicHash::Algorithm hashFunction);
    QString hashString(const QString& str) const;

private:
    QCryptographicHash::Algorithm hashFunction;
};

#endif // HASHMANAGER_H
