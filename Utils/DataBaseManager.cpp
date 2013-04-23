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

    db.driver()->beginTransaction();

    db.exec("create table COMPETITION ( name VARCHAR(80) PRIMARY KEY, place VARCHAR(80), wheel_radius FLOAT)");
    db.exec("create table SECTOR ( id INTEGER PRIMARY KEY AUTOINCREMENT, num INTEGER, ref_compet VARCHAR, min_speed REAL DEFAULT 0 CHECK(min_speed <= max_speed), max_speed REAL DEFAULT 0, start_pos INTEGER, end_pos INTEGER, FOREIGN KEY (ref_compet) REFERENCES COMPETITION(name) ON DELETE CASCADE, FOREIGN KEY (start_pos) REFERENCES POSITION(id) ON DELETE CASCADE, FOREIGN KEY (end_pos) REFERENCES POSITION(id) ON DELETE CASCADE)");
    db.exec("create table RACE ( id INTEGER PRIMARY KEY AUTOINCREMENT, num INTEGER, date DATETIME, ref_compet VARCHAR(80), FOREIGN KEY (ref_compet) REFERENCES COMPETITION(name) ON DELETE CASCADE)");
    db.exec("create table LAP ( num INTEGER, start_time TIME, end_time TIME, distance FLOAT, ref_race INTEGER, FOREIGN KEY (ref_race) REFERENCES RACE(id) ON DELETE CASCADE, PRIMARY KEY (num, ref_race))");
    db.exec("create table SPEED ( id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TIME, value FLOAT, ref_lap_num INTEGER, ref_lap_race  INTEGER, FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race) ON DELETE CASCADE)");
    db.exec("create table ACCELERATION ( id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TIME, g_long FLOAT, g_lat FLOAT, ref_lap_num INTEGER, ref_lap_race  INTEGER, FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race) ON DELETE CASCADE)");
    db.exec("create table POSITION ( id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TIME, latitude FLOAT, longitude FLOAT, altitude FLOAT, eval_speed FLOAT, ref_lap_num INTEGER, ref_lap_race  INTEGER, FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race) ON DELETE CASCADE)");

    return db.driver()->commitTransaction();
}
