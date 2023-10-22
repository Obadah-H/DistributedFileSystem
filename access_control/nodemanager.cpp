#include <nodemanager.h>
#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

NodeManager::NodeManager(const DbManager &dbManager): m_dbManager(dbManager)
{
    m_dbManager.createTable();

}

NodeManager::~NodeManager()
{
}

void NodeManager::migrateNodes(const QStringList& vds) const
{

    foreach(const QString& vd, vds){

        m_dbManager.migrateTable(vd);
    }
}

void NodeManager::compressNodes() const{
    QStringList nodesParents = m_dbManager.getLeavesParents();
    QList<QStringList> nodesToCompress;
    QList<int> nodesToDelete;
    foreach(const QString& path, nodesParents){
        QList<QStringList> parentChilds = m_dbManager.getDirectChilds(path);
        if (parentChilds.count() == 1){
            nodesToCompress << parentChilds [0];
        }
    }
    foreach(QStringList node, nodesToCompress ){
        m_dbManager.compressTwoNodes(node[0].toInt(), node[2]);
    }
    if(nodesToCompress.count()>0 ){
        compressNodes();
    }
}

int NodeManager::convertTreeToBinary(const QString parentPath, int combinerNumber) const{
    QList<QStringList> parentChilds;
    while (true){
        parentChilds = m_dbManager.getDirectChildsByParent(parentPath);
        if(parentChilds.count() <= 2){
            break;
        }
        for(int i=0; i<parentChilds.count(); i+=2){
            if(parentChilds.count() >= i+2){
                m_dbManager.createCombiner({parentChilds[i], parentChilds[i+1]}, combinerNumber);
                combinerNumber++;
            }
        }
    }

    foreach(const QStringList& child, parentChilds){
        combinerNumber = convertTreeToBinary( parentPath + child[1], combinerNumber);
    }
    return combinerNumber;
}
