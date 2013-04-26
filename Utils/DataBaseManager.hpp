#ifndef __DATABASEMANAGER_HPP__
#define __DATABASEMANAGER_HPP__

#include <QtGui>
#include <QtSql>
#include "QException.hpp"

#define DATABASE_KEYWORD "database"
#define DEFAULT_DB_NAME  "EcoMotion.db"

class DataBaseManager
{
    public:

        static bool restorePreviousDataBase(void);

        static bool createDataBase(QString const& dataBaseFilePath);
        static bool createDataBase(QDir const& destDir = QDir::current(),
                                   QString const& dbName = DEFAULT_DB_NAME);

        static bool openExistingDataBase(QString const& dataBaseFilePath);
        static bool openExistingDataBase(QDir const& destDir = QDir::current(),
                                         QString const& dbName = DEFAULT_DB_NAME);

        static void execTransaction(QSqlQuery& query);
        static void execBatch(QSqlQuery& query,
                              QSqlQuery::BatchExecutionMode mode
                              = QSqlQuery::ValuesAsRows);

    private:

        static bool openDataBase(QString const& dataBaseFilePath);
        static bool installDataBase(QString const& dataBaseFilePath);
};

#endif /* __DATABASEMANAGER_HPP__ */
