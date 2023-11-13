#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "dbmanager.h"
#include "dataManager.h"
#include <QSqlDatabase>

/**
 * \class NodeManager
 *
 * \brief Tree Nodes Manager class
 *  */
class NodeManager
{
public:
    /**
     * @brief Constructor
     *
     * Constructor sets up connection with db and opens it
     * @param dbManager - absolute path to db file
     */
    NodeManager(const DataManager& dbManager);

    /**
     * @brief Destructor
     */
    ~NodeManager();

    /**
     * @brief Moves a node from previous VD to the combined tree
     */
    void migrateNode(const QString& vd) const;
    /**
     * @brief Moves all nodes from previous VDs to the combined tree
     */
    void migrateNodes(const QStringList& vds) const;

    /**
     * @brief Compress all nodes which has one child node into one node
     */
    void compressNodes() const;
    /**
     * @brief Converts tree to a binary tree
     */
    int convertTreeToBinary(const QString parentPath, int combinerNumber) const;

    void buildCombinedTree() const;

private:
    DataManager m_dbManager;
};

#endif // DBMANAGER_H
