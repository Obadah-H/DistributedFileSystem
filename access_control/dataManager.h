#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QtCore>
class DataManager{

public:

    virtual ~DataManager() {};

    virtual bool isOpen() const;

    /**
     * @brief Creates a new 'nodes' table if it doesn't already exist
     * @return true - 'nodes' table created successfully, false - table not created
     */
    virtual bool createTable();

    virtual QStringList getLeavesParents() const;

    /**
     * @brief Compress all nodes which has one child node into one node
     */
    virtual void compressTwoNodes(const int id, const QString& path) const;

    virtual QList<QStringList> getDirectChilds(const QString& path) const;

    virtual QList<QStringList> getDirectChildsByParent(const QString& path) const;

    virtual int createCombiner(const QList<QStringList>& dbPath, int combinerNumber) const;
    virtual void fakeVDsTree(const QString& name, int filesCount);
    virtual void fakeVDsTrees(int treesCount);

    virtual void migrateTable(const QString& dbPath) const;
};



#endif // DATAMANAGER_H
