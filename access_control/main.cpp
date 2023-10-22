#include <QCoreApplication>
#include <QDebug>
#include <dbmanager.h>
#include <nodemanager.h>
#include <QTime>


static const QString combinedTreePath = "combined_tree.db";

void fakeVDsTree(const QString& name, int filesCount){
    DbManager db(name);

    if (db.isOpen())
    {
        db.createTable();
        for (int i=0; i<filesCount; i++){
            db.addNode("File"+QString::number(i), "/", "File", "/", "", "R");
        }
    }
}

/**
 * @brief Fakes the creation of VD trees
 */
void fakeVDsTrees(int treesCount){
    for (int i=0; i<treesCount; i++){
        fakeVDsTree("vd"+QString::number(i)+".db", 500);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTime currentTime = QTime::currentTime();
    qDebug() << "Start at:" << currentTime;
    fakeVDsTrees(2);

    DbManager db(combinedTreePath);
    NodeManager node(db);
    node.migrateNodes({"vd0.db", "vd1.db"});
    node.compressNodes();
    node.convertTreeToBinary("/", 1);

    currentTime = QTime::currentTime();
    qDebug() << "Finish at:" << currentTime;

    return a.exec();
}


