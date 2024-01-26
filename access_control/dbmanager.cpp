#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QDir>
#include <QCryptographicHash>

void DbManager::init(const QString &path,  HashManager hashManager)
{
    m_hashManager= hashManager;
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
    this->createTable();
}

void DbManager::fakeVDsTree(const QString& name, int filesCount, int offset){
    DbManager db;
    db.init(name, m_hashManager);

    if (db.isOpen())
    {
        db.createTable();
        for (int i=0; i<filesCount; i++){
            db.addNode("File"+QString::number(i+offset+1), "/", "File", "/", "", "R");
        }
    }
}

/**
 * @brief Fakes the creation of VD trees
 */
void DbManager::fakeVDsTrees(int treesCount){
    for (int i=0; i<treesCount; i++){
        this->fakeVDsTree("vd"+QString::number(i)+".db", 500, i*500);
    }
}


bool DbManager::isOpen() const
{
    return m_db.isOpen();
}

bool DbManager::createTable()
{
    bool success = false;

    QSqlQuery query;
    query.prepare("CREATE TABLE nodes(id INTEGER PRIMARY KEY, name TEXT, path TEXT, type TEXT, parent TEXT, compressed TEXT, hash TEXT, access TEXT);");

    if (!query.exec())
    {
        qDebug() << "Couldn't create the table 'nodes': one might already exist.";
        success = false;
    }

    return success;
}

void DbManager::addNode(const QString& name, const QString& path, const QString& type,  const QString& parent,  const QString& compressed, const QString& access, QString hash )
{
    bool success = false;

    if (!name.isEmpty() && !path.isEmpty() && !type.isEmpty())
    {
        if(hash == "")
        {hash = name+path+type+parent+compressed+access;}

        hash = m_hashManager.hashString(hash);
        QSqlQuery queryAdd;
        queryAdd.prepare("INSERT INTO nodes (name, path, type, parent, compressed, hash, access) VALUES ((:name), (:path), (:type), (:parent), (:compressed), (:hash), (:access))");
        queryAdd.bindValue(":name", name);
        queryAdd.bindValue(":path", path);
        queryAdd.bindValue(":type", type);
        queryAdd.bindValue(":parent", parent);
        queryAdd.bindValue(":compressed", compressed);
        queryAdd.bindValue(":hash", hash);
        queryAdd.bindValue(":access", access);

        if(queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "add file failed: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "add file failed: file info cannot be empty";
    }
}


QStringList DbManager::getRow(QSqlQuery query) const{
    int idId= query.record().indexOf("id");
    int idName = query.record().indexOf("name");
    int idPath = query.record().indexOf("path");
    int idParent = query.record().indexOf("parent");
    int idType = query.record().indexOf("type");
    int idCompressed= query.record().indexOf("compressed");
    int idHash= query.record().indexOf("hash");
    int idAccess= query.record().indexOf("access");

    QString id= query.value(idId).toString();
    QString name = query.value(idName).toString();
    QString path = query.value(idPath).toString();
    QString parent = query.value(idParent).toString();
    QString type = query.value(idType).toString();
    QString compressed = query.value(idCompressed).toString();
    QString hash = query.value(idHash).toString();
    QString access = query.value(idAccess).toString();

    return {id, name, path, type, parent, compressed, hash, access};
}

void DbManager::migrateTable(const QString& dbPath){
    QString originalDB = m_db.databaseName();
    QSqlDatabase srcDB = QSqlDatabase::addDatabase("QSQLITE");
    srcDB.setDatabaseName(dbPath);

    if (!srcDB.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }

    QSqlQuery srcQuery(srcDB);
    if (!srcQuery.exec("SELECT * FROM nodes")) {
        qDebug() << "=== Error DB" <<srcDB.lastError().text();
    }
    QList<QStringList> qlist;

    // Iterate over the result set and insert the data into the destination database
    while (srcQuery.next()) {
        qlist << getRow(srcQuery);
    }

    QSqlDatabase destDB = QSqlDatabase::addDatabase("QSQLITE");
    destDB.setDatabaseName("combined_tree.db");

    if (!destDB.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
    QSqlQuery destQuery(destDB);
    foreach(const QStringList& row, qlist){
        destQuery.prepare("INSERT INTO nodes (name, path, type, parent, compressed, hash, access) VALUES ((:name), (:path), (:type), (:parent), (:compressed), (:hash), (:access))");
        destQuery.bindValue(":name", row[1]);
        destQuery.bindValue(":path", row[2]);
        destQuery.bindValue(":type", row[3]);
        destQuery.bindValue(":parent", row[4]);
        destQuery.bindValue(":compressed", row[5]);
        destQuery.bindValue(":hash", row[6]);
        destQuery.bindValue(":access", row[7]);

        // Insert the data into the destination database
        if (!destQuery.exec()) {
            // Handle error executing insert query
            qDebug() << "=== Error" <<srcDB.lastError().text();
        }else{

        }
    }

    srcDB.setDatabaseName(originalDB);
    qDebug() << "=== Done";
}

QStringList DbManager::getLeavesParents() const{
    QSqlQuery query("SELECT Distinct path FROM nodes where access != ''");
    QStringList ql;
    while (query.next())
    {
        ql << query.value(0).toString();
    }
    return ql;
}

QList<QStringList> DbManager::getDirectChilds(const QString& path) const{
    QSqlQuery query;

    query.prepare("SELECT * FROM nodes where path = (:path) and  path != '/'");
    query.bindValue(":path", path);
    query.exec();

    QList<QStringList> ql;
    while (query.next())
    {
        ql << getRow(query);
    }
    return ql;
}

QList<QStringList> DbManager::getDirectChildsByParent(const QString& parentPath) const{

    QSqlQuery query;
    query.prepare("SELECT * FROM nodes where parent = (:parent)");
    query.bindValue(":parent", parentPath);
    query.exec();
    QList<QStringList> ql;
    while (query.next())
    {
        ql << getRow(query);
    }

    return ql;
}

/**
 * @brief Compress all nodes which has one child node into one node
 */
void DbManager::compressTwoNodes(int id, const QString& path){
    QSqlQuery query;
    QDir dir(path);

    QString parentName = dir.dirName();
    dir.cdUp();  // Move up one directory
    QString parentPath = dir.path();

    query.prepare("DELETE FROM nodes where path = (:path)");
    query.bindValue(":path", parentPath);
    query.exec();
    qDebug() << "Delete:" << parentPath;

    query.prepare("UPDATE nodes set compressed = (:parent) || compressed  and path = (:path) where id = (:id)");
    query.bindValue(":id", id);
    query.bindValue(":path", parentPath);
    query.bindValue(":parent", parentName);
    query.exec();
}


int DbManager::createCombiner(const QList<QStringList>& nodes, int combinerNumber) {
    QStringList node1 = nodes[0];
    QStringList idsList;
    foreach(const QStringList& node, nodes){
        idsList << node[0];
    }
    QStringList node = nodes[0];
    QString ids =idsList.join(",");
    QString combinerName = "Combiner"+QString::number(combinerNumber);
    //insertCombiner
    addNode(combinerName,node[2], "Combiner", node[4],"", "" );
    //Reconnect nodes as combiner childs
    QSqlQuery query;
    query.prepare("UPDATE nodes set parent = '"+combinerName+"' where id in ("+ids+")");
    query.exec();
    //MakeCombinerAsParent
    combinerNumber++;
    return combinerNumber;
};


QStringList DbManager::getNodeByNameAndParent(const QString name, const QString path) {
        QStringList  node;

        QSqlQuery query;
        query.prepare("SELECT * FROM nodes where name = (:name) and  path = (:path) ");
        query.bindValue(":name", name);
        query.bindValue(":path", path);
        query.exec();
        if(query.next())
        {
            node = getRow(query);
        }

        return node;
}
QStringList  DbManager::getNodeBrother(const QStringList& node) {
    QStringList  brother;

    QSqlQuery query;
    query.prepare("SELECT * FROM nodes where name != '"+node[1]+"' and  parent = '"+node[4]+"' ");
    //query.bindValue(":name", node[1]);
    //query.bindValue(":parent", node[4]);
    query.exec();
    if(query.next())
    {
        brother= getRow(query);
    }

    return brother;
}

QStringList DbManager::getNodeParent(const QStringList& node) {

    QStringList  parent;

    QSqlQuery query;
    query.prepare("SELECT * FROM nodes where name = (:name)");
    query.bindValue(":name", node[4]);
    query.exec();
    if(query.next())
    {
        parent= getRow(query);
    }

    return parent;
}
