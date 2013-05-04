#ifndef __DATABASEIMPORTMODULE_HPP__
#define __DATABASEIMPORTMODULE_HPP__

#include <QtGui>
#include <QtSql>
#include "Race.hpp"
#include "GeoCoordinate.hpp"
#include "../RaceViewer.hpp"
#include "../Utils/QException.hpp"
#include "../Utils/QCSVParser.hpp"
#include "../Utils/DataBaseManager.hpp"
#include "../Megasquirt/MSManager.hpp"

#define GPS_KEY             "GPS"
#define SPEED_KEY           "SPEED"
#define MEGASQUIRT_DAT_KEY  "MEGASQUIRT_DAT"
#define MEGASQUIRT_CSV_KEY  "MEGASQUIRT_CSV"

#define DEFAULT_GPS_FILENAME            "gps.csv"
#define DEFAULT_SPEED_FILENAME          "speed.csv"
#define DEFAULT_MEGASQUIRT_DAT_FILENAME "megasquirt.dat"
#define DEFAULT_MEGASQUIRT_CSV_FILENAME "megasquirt.csv"

class DataBaseImportModule : public QObject
{
    Q_OBJECT

    public:

        explicit DataBaseImportModule(void);
        virtual ~DataBaseImportModule(void);

        void checkFolderContent(QDir const& dataDirectory) throw(QException);

        void createCompetition(const QString& name, float wheel_radius = 0.0,
                               const QString& place = QString());

        void addRace(Race& race, QDir const& dataDirectory) throw(QException);

    protected:

        void loadConfiguration(void);

        void createRace(Race& race);
        void deleteRace(const Race& race);

        int createLap(Race& race, int num, QTime const& start, QTime const& end);

        void loadGPSData(QString const& GPSFilePath, Race &race);
        void loadSpeedData(QString const& speedFilePath, Race &race);
        void loadSpeedData_2(QString const& speedFilePath, Race &race);
        void loadMegasquirtData(QString const& megasquirtDATFilePath,
                                QString const& megasquirtCSVFilePath,
                                Race& race);

    protected:

        QString _gpsFileName;
        QString _speedFileName;
        QString _MegasquirtDATFileName;
        QString _MegasquirtCSVFileName;
};

#endif /* __DATABASEIMPORTMODULE_HPP__ */
