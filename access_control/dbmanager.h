#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "datamanager.h"
#include "hashmanager.h"
#include <QSqlDatabase>

class DbManager: public DataManager
{
public:
    void init(const QString &path,  HashManager hashManager) override;

    void fakeVDsTree(const QString& name, int filesCount, int offset) override;
    void fakeVDsTrees(int treesCount) override;


    bool isOpen() const;
    bool createTable() override;
    void addNode(const QString& name, const QString& path, const QString& type,  const QString& parent,  const QString& compressed, const QString& access, QString hash = "") override;


    QStringList getLeavesParents() const override;

    void compressTwoNodes(const int id, const QString& path) override;

    QList<QStringList> getDirectChilds(const QString& path) const override;

    QList<QStringList> getDirectChildsByParent(const QString& path) const override;

    int createCombiner(const QList<QStringList>& nodes, int combinerNumber) override;

    void migrateTable(const QString& dbPath) override;
    QStringList getRow(QSqlQuery query) const;


    QStringList getNodeByNameAndParent(const QString name, const QString path) override;
    QStringList getNodeBrother(const QStringList& node) override;
    QStringList getNodeParent(const QStringList& node) override;

private:
    QSqlDatabase m_db;
    HashManager m_hashManager;
};

#endif // DBMANAGER_H
