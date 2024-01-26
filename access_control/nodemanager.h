#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "datamanager.h"

class NodeManager
{
public:
    /**
     * @brief Constructor
     *
     * Constructor sets up connection with db and opens it
     * @param dbManager - absolute path to db file
     */
    NodeManager();

    /**
     * @brief Destructor
     */
    ~NodeManager();
    void setNodeDataManager(DataManager& dManager);

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
        QString getNodeProof(const QString nodeName, const QString nodePath) const;

        void setTreeToBinary(const QString parentPath, int combinerNumber) const;
        void buildCombinedTree() const;
        void init(DataManager* dm);
private:
        DataManager* m_dataManager;
};

#endif // NODEMANAGER_H
