#include "nodemanager.h"
#include "dbmanager.h"
#include <QSqlDatabase>


NodeManager::NodeManager()
{}
NodeManager::~NodeManager()
{
}

void NodeManager::init(DataManager* dm){
    m_dataManager = dm;
}



void NodeManager::setNodeDataManager(DataManager& dManager){
    dManager.createTable();
}

void NodeManager::migrateNodes(const QStringList& vds) const
{

    foreach(const QString& vd, vds){

        m_dataManager->migrateTable(vd);
    }
    m_dataManager->finishMigration();
}

void NodeManager::compressNodes() const{
    QStringList nodesParents = m_dataManager->getLeavesParents();
    QList<QStringList> nodesToCompress;
    QList<int> nodesToDelete;
    foreach(const QString& path, nodesParents){
        QList<QStringList> parentChilds = m_dataManager->getDirectChilds(path);
        if (parentChilds.count() == 1){
            nodesToCompress << parentChilds [0];
        }
    }
    foreach(QStringList node, nodesToCompress ){
        m_dataManager->compressTwoNodes(node[0].toInt(), node[2]);
    }
    if(nodesToCompress.count()>0 ){
        compressNodes();
    }
}

void NodeManager::setTreeToBinary(const QString parentPath, int combinerNumber) const{
    convertTreeToBinary(parentPath, combinerNumber);

    qDebug() <<"Done!";
    m_dataManager->finishMigration();
}
int NodeManager::convertTreeToBinary(const QString parentPath, int combinerNumber) const{
    QList<QStringList> parentChilds;
    while (true){
        parentChilds = m_dataManager->getDirectChildsByParent(parentPath);
        if(parentChilds.count() <= 2){
            break;
        }
        for(int i=0; i<parentChilds.count(); i+=2){
            if(parentChilds.count( ) >= i+2){
                m_dataManager->createCombiner({parentChilds[i], parentChilds[i+1]}, combinerNumber);
                combinerNumber++;
            }
        }
    }

    return combinerNumber;


    foreach(const QStringList& child, parentChilds){
        combinerNumber = convertTreeToBinary( parentPath + child[1], combinerNumber);
    }
    return combinerNumber;
}

QString NodeManager::getNodeProof(const QString nodeName, const QString nodePath) const{
    QStringList proof;
    QStringList node = m_dataManager->getNodeByNameAndParent(nodeName, nodePath);
    proof.append(node.join(""));
    while(true){
    QStringList brother = m_dataManager->getNodeBrother(node);
    //qDebug() << node << brother;
    proof.append(brother[6]);
    // If parent is root
    if(node[4] == "/"){
        break;
    }
    node = m_dataManager->getNodeParent(node);
    }
    return proof.join(",");
}
