#include "DataBaseManager.hpp"

bool DataBaseManager::restorePreviousDataBase(void)
{
    QSettings settings;

    if(settings.contains(DATABASE_KEYWORD))
    {
        QString dbFilePath = settings.value(DATABASE_KEYWORD).toString();

        if (QFile::exists(dbFilePath))
            return DataBaseManager::openDataBase(dbFilePath);
    }

    return false;
}

bool DataBaseManager::createDataBase(const QString &dataBaseFilePath)
{
    if (!DataBaseManager::installDataBase(dataBaseFilePath))
        return false;

    // Save the database name
    QSettings settings;
    settings.setValue(DATABASE_KEYWORD, dataBaseFilePath);

    return true;
}

bool DataBaseManager::createDataBase(QDir const& destDir, QString const& dbName)
{
    return DataBaseManager::createDataBase(destDir.filePath(dbName));
}

bool DataBaseManager::openExistingDataBase(const QString &dataBaseFilePath)
{
    if(!DataBaseManager::openDataBase(dataBaseFilePath))
        return false;

    // Save the database name
    QSettings settings;
    settings.setValue(DATABASE_KEYWORD, dataBaseFilePath);

    return true;
}

bool DataBaseManager::openExistingDataBase(const QDir &destDir,
                                           const QString &dbName)
{
    return DataBaseManager::openExistingDataBase(destDir.filePath(dbName));
}

QSqlQuery DataBaseManager::execQuery(const QString &queryString,
                                     const QVariantList &values, bool forwardOnly)
{
    QSqlQuery query(queryString);

    // bind values
    foreach (QVariant value, values)
        query.addBindValue(value);

    query.setForwardOnly(forwardOnly);

    if (!query.exec())
        throw QException(QObject::tr("La requete a échouée : ")
                         + query.lastQuery() + query.lastError().text());

    return query; // Implicit sharing
}

void DataBaseManager::execTransaction(QSqlQuery &query)
{
    QSqlDriver* sqlDriver = QSqlDatabase::database().driver();

    sqlDriver->beginTransaction();

    if(!query.exec())
    {
        sqlDriver->rollbackTransaction();
        throw QException(QObject::tr("la requête a échouée : ")
                         + query.lastQuery() + query.lastError().text());
    }

    // Try to commit transaction
    if(!sqlDriver->commitTransaction())
        throw QException(QObject::tr("la validation des données à échouée"));
}

void DataBaseManager::execBatch(QSqlQuery &query,
                                QSqlQuery::BatchExecutionMode mode)
{
    QSqlDriver* sqlDriver = QSqlDatabase::database().driver();

    sqlDriver->beginTransaction();

    if(!query.execBatch(mode))
    {
        sqlDriver->rollbackTransaction();
        throw QException(QObject::tr("la requête a échouée : ")
                         + query.lastQuery() + query.lastError().text());
    }

    // Try to commit transaction
    if(!sqlDriver->commitTransaction())
        throw QException(QObject::tr("la validation des données à échouée"));
}

bool DataBaseManager::openDataBase(const QString& dataBaseFilePath)
{
    // Close previous connection if exists
//    QSqlDatabase currentDB = QSqlDatabase::database();
//    if(currentDB.isValid() && currentDB.isOpen())
//    {
//        currentDB.close();
//        QSqlDatabase::removeDatabase(currentDB.connectionName());
//    }

    if(QSqlDatabase::database().isValid() && QSqlDatabase::database().isOpen())
    {
        QString dbConnectionName = QSqlDatabase::database().connectionName();

        QSqlDatabase::database().close();
        QSqlDatabase::removeDatabase(dbConnectionName);
    }

//    QString dbConnectionName;
//    {
//        QSqlDatabase db = QSqlDatabase::database();
//        if(db.isValid() && db.isOpen())
//        {
//            dbConnectionName = db.connectionName();
//            db.close();
//        }
//    }

//    QSqlDatabase::removeDatabase(dbConnectionName);


    /* ---------------------------------------------------------------------- *
     *                             Open database                              *
     * ---------------------------------------------------------------------- */

    qDebug() << "Ouverture de la base de données : " << dataBaseFilePath;

    /* if connectionName is not specified, the new connection becomes the
     * default connection for the application */
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataBaseFilePath);

    if (!db.open())
        throw QException(QObject::tr("Impossible d'ouvrir' la base ") +
                         dataBaseFilePath + db.lastError().text());

    db.exec("PRAGMA foreign_keys = ON");
    db.exec("PRAGMA journal_mode = MEMORY");
    db.exec("PRAGMA synchronous  = OFF");

    return true;
}

bool DataBaseManager::installDataBase(QString const& dataBaseFilePath)
{
    DataBaseManager::openDataBase(dataBaseFilePath);

    /* ---------------------------------------------------------------------- *
     *                              Create tables                             *
     * ---------------------------------------------------------------------- */
    QSqlDatabase db = QSqlDatabase::database();

    // Use a script to create tables
    QFile schemaFile(":/sql/schemaTable"); // Resource file

    // get all table creation script
    schemaFile.open(QFile::ReadOnly);
    QStringList schemaTableList = QString(schemaFile.readAll()).split(";", QString::SkipEmptyParts);

    db.driver()->beginTransaction();

    // Create all table from script
    foreach(const QString schemaTable, schemaTableList)
    {
        if(!schemaTable.trimmed().isEmpty())
        {
            qDebug() << "Creation d'une table --------------------------------";
            qDebug() << schemaTable;
            db.exec(schemaTable);
        }
    }

    bool commitSucced = db.driver()->commitTransaction();

    schemaFile.close();

    return commitSucced;
}
