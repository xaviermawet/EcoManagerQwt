/* NOTE:
 *
 * QSqlQuery
 *-----------
 * If you only need to move forward through the results (e.g., by using next()),
 * you can use setForwardOnly(), which will save a significant amount of memory
 * overhead and improve performance on some databases.
 *
 */

#ifndef __DATABASEMANAGER_HPP__
#define __DATABASEMANAGER_HPP__

#include <QtGui>
#include <QtSql>
#include "QException.hpp"

#define DATABASE_KEYWORD "database"

class DataBaseManager
{
    public:

        static bool restorePreviousDataBase(void);

        static bool createDataBase(QString const& dataBaseFilePath);
        static bool createDataBase(QDir const& destDir = QDir::current(),
                                   QString const& dbName = "EcoMotion.db");

        static bool openExistingDataBase(QString const& dataBaseFilePath);
        static bool openExistingDataBase(QDir const& destDir = QDir::current(),
                                         QString const& dbName = "EcoMotion.db");

    private:

        static bool openDataBase(QString const& dataBaseFilePath);
        static bool installDataBase(QString const& dataBaseFilePath);
};

#endif /* __DATABASEMANAGER_HPP__ */
