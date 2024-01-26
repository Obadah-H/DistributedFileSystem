#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include "datamanager.h"
#include "hashmanager.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>


class JsonManager: public DataManager
{
public:
    void init(const QString &path,  HashManager hashManager) override;
    bool createTable() override;
    void addNode(const QString& name, const QString& path, const QString& type,  const QString& parent,  const QString& compressed, const QString& access,QString hash = "");
    void fakeVDsTree(const QString& name, int filesCount, int offset) override;
    void fakeVDsTrees(int treesCount) override;
    QStringList getLeavesParents() const override;
    void compressTwoNodes(const int id, const QString& path) override;
    QList<QStringList> getDirectChilds(const QString& path) const override;
    QList<QStringList> getDirectChildsByParent(const QString& path) const override;
    int createCombiner(const QList<QStringList>& nodes, int combinerNumber) override;
    void migrateTable(const QString& dbPath) override;
    void saveTree(const QString& path);
    QJsonArray readFile(const QString& path) const;
    QStringList getRow(QJsonObject obj) const;
    void finishMigration() override;

    QStringList getNodeByNameAndParent(const QString name, const QString path) override;
    QStringList getNodeBrother(const QStringList& node) override;
    QStringList getNodeParent(const QStringList& node) override;


private:
    HashManager m_hashManager;
    QJsonArray m_jsonArray;
    QString m_jsonPath;

};

#endif // JSONMANAGER_H
