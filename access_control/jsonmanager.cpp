#include "jsonmanager.h"

void JsonManager::init(const QString &path,  HashManager hashManager) {
    m_hashManager=hashManager;
    m_jsonPath = path;
    m_jsonArray = readFile(path);
}
bool JsonManager::createTable() {}
void JsonManager::fakeVDsTree(const QString& name, int filesCount, int offset) {
    JsonManager _jm;
    _jm.init(name, m_hashManager);

    for (int i = 0; i < filesCount; i++) {
        _jm.addNode("File"+QString::number(i+offset+1), "/", "File", "/", "", "R");
    }
    _jm.saveTree(name);
}
void JsonManager::fakeVDsTrees(int treesCount) {

    int treeSize = 500;
    for (int i=0; i<treesCount; i++){
        this->fakeVDsTree("vd"+QString::number(i)+".db", treeSize, i*treeSize);
    }
}
void JsonManager::addNode(const QString& name, const QString& path, const QString& type,  const QString& parent,  const QString& compressed, const QString& access, QString hash )
{

    if (hash == ""){
        hash = name+path+type+parent+compressed+access;
    }
    hash = m_hashManager.hashString(hash);
    QJsonObject jsonObject;
   int id = 1;

   if(m_jsonArray.count() > 0){
     id =   (m_jsonArray[m_jsonArray.count()-1].toObject()["id"].toString()).toInt() + 1;
   }

    jsonObject.insert("id", QString::number(id));
    jsonObject.insert("name", name);
    jsonObject.insert("path", path);
    jsonObject.insert("type", type);
    jsonObject.insert("parent", parent);
    jsonObject.insert("compressed", compressed);
    jsonObject.insert("hash", hash);
    jsonObject.insert("access", access);
    m_jsonArray.append(jsonObject);
}
QStringList JsonManager::getLeavesParents() const {
    QStringList ql;
    for (const QJsonValue& value : m_jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (!ql.contains(obj["path"].toString()) && obj["access"].toString() != "") {
                ql << obj["path"].toString();
            }
        }
    }
    return ql;
}
void JsonManager::compressTwoNodes(const int id, const QString& path) {

    QDir dir(path);

    QString parentName = dir.dirName();
    dir.cdUp();  // Move up one directory
    QString parentPath = dir.path();
    int arraySize = m_jsonArray.size();
    for (int i = arraySize-1; i >= 0; i--) {
        QJsonObject jsonObject = m_jsonArray[i].toObject();
        if (jsonObject["path"].toString() == parentPath) {
            // Remove the object from the array
            m_jsonArray.removeAt(i);
        }else{
            if ((int) jsonObject["id"].toInt() == id) {
                jsonObject["compressed"] = parentName+"/"+jsonObject["compressed"].toString();
                jsonObject["path"] = parentPath;
                m_jsonArray[i] = jsonObject;
            }
        }
    }
}
QList<QStringList> JsonManager::getDirectChilds(const QString& path) const {
    QList<QStringList> ql;
    for (const QJsonValue& value : m_jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj["path"].toString() == path && obj["path"].toString() != "/") {
                ql << getRow(obj);
            }
        }
    }
    return ql;}
QList<QStringList> JsonManager::getDirectChildsByParent(const QString& path) const {
    QList<QStringList> ql;
    for (const QJsonValue& value : m_jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            if (obj["parent"].toString() == path ) {
                ql << getRow(obj);
            }
        }
    }
    return ql;}

int JsonManager::createCombiner(const QList<QStringList>& nodes, int combinerNumber) {
    int counter = 0;
    QString id1 = nodes[0][0];
    QString id2 = nodes[1][0];

    QString hash1 = nodes[0][6];
    QString hash2 = nodes[1][6];

    QStringList node = nodes[0];
    QString combinerName = "Combiner"+QString::number(combinerNumber);
    //insertCombiner
    addNode(combinerName,node[2], "Combiner", node[4],"", "" , hash1+hash2);


    for (int i = 0; i < m_jsonArray.size(); i++) {
        QJsonObject obj = m_jsonArray[i].toObject();
            if (obj["id"].toString() == id1 || obj["id"].toString() == id2) {
                obj["parent"] = combinerName;
                m_jsonArray[i] = obj;

                counter++;
            }
            if(counter == 2){
                break;
            }
    }

    combinerNumber++;
    return combinerNumber;}

void JsonManager::finishMigration() {
    /*
    //Add id field
     int idCounter = 1;
    // Iterate through the array to assign IDs to each object
    for (int i = 0; i < m_jsonArray.size(); i++) {
        QJsonObject jsonObject = m_jsonArray[i].toObject();

        // Add the "id" attribute with the auto-incremented value
        jsonObject["id"] = QString::number( idCounter++);

        // Replace the object in the array with the updated object
        m_jsonArray[i] = jsonObject;
    }
    */
    saveTree(m_jsonPath);
};
void JsonManager::migrateTable(const QString& dbPath)  {

    QJsonArray fromFileArr = readFile(dbPath);

    int id = 1;
    if(m_jsonArray.count() > 0){
      id =  (m_jsonArray[m_jsonArray.count()-1].toObject()["id"].toString()).toInt() + 1;
    }
    for (const QJsonValue& value : fromFileArr) {
         QJsonObject obj = value.toObject();
         obj["id"] =QString::number(id);
        m_jsonArray.append(obj);
        id++;
    }
}

QJsonArray JsonManager::readFile(const QString& path) const{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Error opening file";
    }else{
        qDebug()<<"Reading file";
    }
    if(file.exists()){
        QString fileContents = file.readAll();
       file.close();
       QJsonDocument jsonDocument = QJsonDocument::fromJson(fileContents.toUtf8());
       if (jsonDocument.isArray())
       {
           return jsonDocument.array();
       }
    }
    return {};
}

void JsonManager::saveTree(const QString& path){
    QJsonDocument jsonDocument(m_jsonArray);
    QByteArray jsonData = jsonDocument.toJson();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
       // Handle error
       qDebug() << "Failed to open file";
       return;
    }else{
        qDebug() << "writing to "<<path;
    }

    file.write(jsonData);
    file.close();
}


QStringList JsonManager::getRow(QJsonObject obj) const{
    return {obj["id"].toString(), obj["name"].toString(), obj["path"].toString(), obj["type"].toString(), obj["parent"].toString(),
                obj["compressed"].toString(), obj["hash"].toString(), obj["access"].toString()};
}


QStringList JsonManager::getNodeByNameAndParent(const QString name, const QString path) {
    QStringList  node;
    for (int i = 0; i < m_jsonArray.size(); i++) {
        QJsonObject obj = m_jsonArray[i].toObject();
            if (obj["name"].toString() == name && obj["path"].toString() == path) {
                node = getRow(obj);
                break;
            }
    }
    return node;
};
QStringList  JsonManager::getNodeBrother(const QStringList& node) {

    QStringList  brother;
    for (int i = 0; i < m_jsonArray.size(); i++) {
        QJsonObject obj = m_jsonArray[i].toObject();
            if (obj["name"].toString() != node[1] && obj["parent"].toString() == node[4]) {
                brother= getRow(obj);
                break;
            }
    }
    return brother;
};


QStringList JsonManager::getNodeParent(const QStringList& node) {

    QStringList  parent;
    for (int i = 0; i < m_jsonArray.size(); i++) {
        QJsonObject obj = m_jsonArray[i].toObject();
            if (obj["name"].toString() == node[4] ) {
                parent= getRow(obj);
                break;
            }
    }
    return parent;
}
