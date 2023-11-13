#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "dataManager.h"
#include <QSqlDatabase>

/**
 * \class DbManager
 *
 * \brief SQL Database Manager class
 */
class DbManager : public DataManager
{
public:
    /**
     * @brief Constructor
     *
     * Constructor sets up connection with db and opens it
     * @param path - absolute path to db file
     */
    DbManager(const QString& path);

    /**
     * @brief Destructor
     *
     * Close the db connection
     */
    ~DbManager();

    bool isOpen() const;

    /**
     * @brief Creates a new 'nodes' table if it doesn't already exist
     * @return true - 'nodes' table created successfully, false - table not created
     */
    bool createTable() override;

    QSqlQuery getAllNodes() const;

    QSqlQuery getAllPaths() const;
    /**
     * @brief Add node to db
     */
    bool addNode(const QString& name, const QString& path, const QString& type,  const QString& parent,  const QString& compressed, const QString& access) const;

    QString hashString(const QString& str) const;

    void getLeaves() const;

    QStringList getLeavesParents() const override;

    /**
     * @brief Compress all nodes which has one child node into one node
     */
    void compressTwoNodes(const int id, const QString& path) const override;

    QList<QStringList> getDirectChilds(const QString& path) const override;

    QList<QStringList> getDirectChildsByParent(const QString& path) const override;

    /**
     * @brief get row from sql query
     */
    QStringList getRow(QSqlQuery query) const;

    int createCombiner(const QList<QStringList>& dbPath, int combinerNumber) const override;

    void migrateTable(const QString& dbPath) const override;

    QString getDbPath() const;

    void fakeVDsTree(const QString& name, int filesCount) override;
    void fakeVDsTrees(int treesCount) override;

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H