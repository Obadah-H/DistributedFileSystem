#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "hashmanager.h"
#include "QtCore"


class DataManager
{
public:
    virtual void init(const QString &path, HashManager hashManager) {};
    virtual void fakeVDsTree(const QString& name, int filesCount, int offset) {};
    virtual void fakeVDsTrees(int treesCount) {};


    /**
     * @brief Creates a new 'nodes' table if it doesn't already exist
     * @return true - 'nodes' table created successfully, false - table not created
     */
    virtual bool createTable() =0;


    virtual QStringList getLeavesParents() const =0;

    virtual void compressTwoNodes(const int id, const QString& path){};

    virtual QList<QStringList> getDirectChilds(const QString& path) const = 0;

    virtual QList<QStringList> getDirectChildsByParent(const QString& path) const =0;

    virtual int createCombiner(const QList<QStringList>& nodes, int combinerNumber) =0;

    virtual void migrateTable(const QString& dbPath) {};
    virtual void finishMigration() {};
    virtual void addNode(const QString& name, const QString& path, const QString& type,  const QString& parent,  const QString& compressed, const QString& access,  QString hash = "") =0;

    //Merkle Proof
    virtual QStringList getNodeByNameAndParent(const QString name, const QString path) = 0;
    virtual QStringList getNodeBrother(const QStringList& node) =0;
    virtual QStringList getNodeParent(const QStringList& node) =0;
};

#endif // DATAMANAGER_H
