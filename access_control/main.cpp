#include "datamanager.h"
#include "dbmanager.h"
#include "nodemanager.h"
#include "jsonmanager.h"
#include "hashmanager.h"
#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>

void fakeVDJsonTest(){

    QFile::remove("vd0.db");
    QFile::remove("vd1.db");
    QFile::remove("combined_tree.db");
    DataManager* dm = new JsonManager();
    HashManager hm(QCryptographicHash::Sha256);

    NodeManager node;
    dm->init("combined_tree.db", hm);
   dm->fakeVDsTrees(2);
}

void fakeVDSQLiteTest(){

    QFile::remove("vd0.db");
    QFile::remove("vd1.db");
    QFile::remove("combined_tree.db");
    DataManager* dm = new DbManager();

    HashManager hm(QCryptographicHash::Sha256);

    NodeManager node;
    dm->init("combined_tree.db", hm);
   dm->fakeVDsTrees(2);
}

void createTreeSQLiteTest(){
    DataManager* dm = new DbManager();

    HashManager hm(QCryptographicHash::Sha256);

    NodeManager node;
    dm->init("combined_tree.db", hm);
    node.init(dm);
    node.migrateNodes({"vd0.db", "vd1.db"});
    node.compressNodes();
    node.setTreeToBinary("/", 1);
}


void createTreeJsonTest(){
    DataManager* dm = new JsonManager();

    HashManager hm(QCryptographicHash::Sha256);
    dm->init("combined_tree.db", hm);

    NodeManager node;
    node.init(dm);
    node.migrateNodes({"vd0.db", "vd1.db"});
    node.compressNodes();
    node.setTreeToBinary("/", 1);
}


void updateTreeSQLiteTest(){
    //No migration here

    //1- add a new node to one of the vds
    HashManager hm(QCryptographicHash::Sha256);
    DataManager* _dm = new DbManager();

    _dm->init("vd0.db", hm);

    //Add a new node to the combined tree
    _dm->addNode("newFile", "/", "File", "/", "", "R");

    //Optimize tree
    DataManager* dm = new DbManager();
    NodeManager node;
    dm->init("combined_tree.db", hm);

    //Add a new node
    dm->addNode("newFile", "/", "File", "/", "", "R");

    node.init(dm);
    node.compressNodes();
    node.setTreeToBinary("/", 1);
}


void updateTreeJsonTest(){
    //No migration here
    // 1- add new entry to one of the vds

    QJsonObject newObject;
    newObject["access"] = "R";
    newObject["compressed"] = "";
    newObject["hash"] = "603e668c601ae0818c692b89b13cd81939db20fcba0ddc7799291f868bebf335";
    newObject["id"] = "1999";
    newObject["name"] = "newFile";
    newObject["parent"] = "/";
    newObject["path"] = "/";
    newObject["type"] = "File";
    QFile _file("combined_tree.db");
        if (_file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QByteArray jsonData = _file.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(jsonData);
            QJsonArray jsonArray = doc.array();
            jsonArray.append(newObject);
            _file.resize(0); // Clear the file content
            _file.write(QJsonDocument(jsonArray).toJson());
            _file.close();
        }

        // 2- add new entry to the combined tree
        QFile file("combined_tree.db");
            if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                QByteArray jsonData = file.readAll();
                QJsonDocument doc = QJsonDocument::fromJson(jsonData);
                QJsonArray jsonArray = doc.array();
                jsonArray.append(newObject);
                file.resize(0); // Clear the file content
                file.write(QJsonDocument(jsonArray).toJson());
                file.close();
            }

    //2-optimize combined tree

    DataManager* dm = new JsonManager();

    HashManager hm(QCryptographicHash::Sha256);
    NodeManager node;
    dm->init("combined_tree.db", hm);
    node.init(dm);
    node.compressNodes();
    node.setTreeToBinary("/", 1);
}


void getMerkleProofSQLite(){
    DataManager* dm = new DbManager();

    HashManager hm(QCryptographicHash::Sha256);
    dm->init("combined_tree.db", hm);

    NodeManager node;
    node.init(dm);
    qDebug()<<node.getNodeProof("File1", "/");
}


void getMerkleProofJson(){
    DataManager* dm = new JsonManager();

    HashManager hm(QCryptographicHash::Sha256);
    dm->init("combined_tree.db", hm);

    NodeManager node;
    node.init(dm);
    qDebug()<<node.getNodeProof("File1", "/");
}

int main(int argc, char *argv[])
{

    QTime currentTime = QTime::currentTime();
    qDebug() << "Start at:" << currentTime;

    //Choose one of these tests at time!

    //fakeVDJsonTest();
    //createTreeJsonTest();
    //updateTreeJsonTest();
    getMerkleProofJson();


    //fakeVDSQLiteTest();
    //createTreeSQLiteTest();
   //updateTreeSQLiteTest();
    //getMerkleProofSQLite();

   currentTime = QTime::currentTime();
   qDebug() << "Finish at:" << currentTime;
    return 0;
}

