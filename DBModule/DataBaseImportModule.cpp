#include "DataBaseImportModule.hpp"

DataBaseImportModule::DataBaseImportModule(void)
{
    // Check if the data base is open, otherwise import isn't possible
    if (!QSqlDatabase::database().isOpen())
        throw QException(tr("La base de données ")  +
                         QSqlDatabase::database().databaseName() +
                         tr(" n'est pas ouverte. Importation impossible"));

    // The data base is opened, load name of files to import
    this->loadConfiguration();
}

DataBaseImportModule::~DataBaseImportModule(void)
{
    // Nothing to do ...
}

void DataBaseImportModule::checkFolderContent(const QDir &dataDirectory) throw(QException)
{
    // GPS
    if(!dataDirectory.exists(this->_gpsFileName))
        throw QException(tr("Le fichier ") + this->_gpsFileName +
                         tr(" est manquant"));

    // SPEDD
    if(!dataDirectory.exists(this->_speedFileName))
        throw QException(tr("Le fichier ") + this->_speedFileName +
                         tr(" est manquant"));

    // MEGASQUIRT
    if(!dataDirectory.exists(this->_MegasquirtDATFileName))
        throw QException(tr("Le fichier ") + this->_MegasquirtDATFileName +
                         tr(" est manquant"));
}

void DataBaseImportModule::createCompetition(
        const QString &name, float wheel_radius, const QString &place)
{
    QSqlQuery createCompetitionQuery;
    createCompetitionQuery.prepare
            (
                "insert into COMPETITION (name, wheel_radius, place) "
                "values (?, ?, ?)"
            );
    createCompetitionQuery.addBindValue(name);
    createCompetitionQuery.addBindValue(wheel_radius);
    createCompetitionQuery.addBindValue(place);

    // Execute insert in transaction
    DataBaseManager::execTransaction(createCompetitionQuery);
}

void DataBaseImportModule::addRace(Race &race, const QDir &dataDirectory) throw(QException)
{
    qDebug() << "Insertion des données pour la course...";

    this->checkFolderContent(dataDirectory);

    // Create an entry for the race in the database
    this->createRace(race);

    try
    {
        /* A partir d'ici, une entrée pour la course existe dans la base de
         * données. Mais s'il y a une erreur lors de l'ajout de données
         * (GPS ou SPEED ou Megasquirt), il faut supprimer l'entrée pour la
         * course et tous les autres tuples qui référencient cette course */
        this->loadGPSData(dataDirectory.filePath(this->_gpsFileName), race);
        this->loadSpeedData(dataDirectory.filePath(this->_speedFileName), race);
        this->loadSpeedData_2(dataDirectory.filePath(this->_speedFileName), race);
        this->loadMegasquirtData(
                    dataDirectory.filePath(this->_MegasquirtDATFileName),
                    dataDirectory.filePath(this->_MegasquirtCSVFileName),
                    race);
    }
    catch(QException const& ex)
    {
        this->deleteRace(race);
        throw QException(ex);
    }
}

void DataBaseImportModule::loadConfiguration(void)
{
    QSettings settings;

    settings.beginGroup("Files");

    if(!settings.contains(GPS_KEY))
        settings.setValue(GPS_KEY, DEFAULT_GPS_FILENAME);
    this->_gpsFileName = settings.value(GPS_KEY).toString();

    if(!settings.contains(SPEED_KEY))
        settings.setValue(SPEED_KEY, DEFAULT_SPEED_FILENAME);
    this->_speedFileName = settings.value(SPEED_KEY).toString();

    if(!settings.contains(MEGASQUIRT_DAT_KEY))
        settings.setValue(MEGASQUIRT_DAT_KEY, DEFAULT_MEGASQUIRT_DAT_FILENAME);
    this->_MegasquirtDATFileName = settings.value(MEGASQUIRT_DAT_KEY).toString();

    if(!settings.contains(MEGASQUIRT_CSV_KEY))
        settings.setValue(MEGASQUIRT_CSV_KEY, DEFAULT_MEGASQUIRT_CSV_FILENAME);
    this->_MegasquirtCSVFileName = settings.value(MEGASQUIRT_CSV_KEY).toString();

    settings.endGroup();
}

void DataBaseImportModule::createRace(Race& race)
{
    /* ---------------------------------------------------------------------- *
     *                    Get wheel perimeter for the race                    *
     * ---------------------------------------------------------------------- */
    QSqlQuery wheelQuery("select wheel_radius from COMPETITION where name = ?");
    wheelQuery.addBindValue(race.competition());

    if(!wheelQuery.exec() || !wheelQuery.next())
       throw QException(wheelQuery.lastQuery() + wheelQuery.lastError().text());

    qreal perimeter = wheelQuery.value(0).toDouble();
    race.setWheelPerimeter(perimeter);

    /* ---------------------------------------------------------------------- *
     *                         Get the new race number                        *
     * ---------------------------------------------------------------------- */
    QSqlQuery numQuery(QString("select max(num) from RACE where ref_compet = \"%1\"").arg(race.competition()));
    int numRace = 1;

    if(!numQuery.exec() || !numQuery.next())
        throw QException(numQuery.lastQuery() + numQuery.lastError().text());
    numRace = numQuery.value(0).toInt() + 1;

    /* ---------------------------------------------------------------------- *
     *                        Insert race in data base                        *
     * ---------------------------------------------------------------------- */
    QSqlQuery query;
    query.prepare("insert into RACE (num, date, ref_compet) values (?, ?, ?)");
    query.addBindValue(numRace);
    query.addBindValue(race.date());
    query.addBindValue(race.competition());

    DataBaseManager::execTransaction(query); // Execute query in transaction
    race.setId(query.lastInsertId().toInt());
}

void DataBaseImportModule::deleteRace(const Race &race)
{
    QSqlQuery query;
    query.prepare("delete from RACE where id = ?");
    query.addBindValue(race.id());

    DataBaseManager::execTransaction(query);
    qDebug() << "delete empty race [OK]";
}

int DataBaseImportModule::createLap(
        Race &race, int num, const QTime &start, const QTime &end)
{
    QSqlQuery query;
    query.prepare("insert into LAP (num, ref_race, start_time, end_time) values (?, ?, ?, ?)");

    QTime origin(0, 0);
    int startstamp = origin.msecsTo(start);
    int endstamp = origin.msecsTo(end);

    query.addBindValue(num);
    query.addBindValue(race.id());
    query.addBindValue(startstamp);
    query.addBindValue(endstamp);

    race.addLap(start, end);
    int rc(-1);

    if (!query.exec())
        throw QException(tr("L'ajout d'un tour à échoué"));
    else
        rc = query.lastInsertId().toInt();

    return rc;
}

void DataBaseImportModule::loadGPSData(const QString& GPSFilePath, Race &race)
{
    qDebug() << "Chargement des données GPS...";

    QFile GPSFile(GPSFilePath);
    if(!GPSFile.open(QIODevice::ReadOnly))
        throw QException(tr("Impossible d'ouvrir le fichier ") + GPSFilePath);

    QTextStream in(&GPSFile);
    QSqlQuery query;
    query.prepare("insert into POSITION (timestamp, longitude, latitude, altitude, eval_speed, ref_lap_race, ref_lap_num) values (?, ?, ?, ?, ?, ?, ?)");
    QVariantList timestamps;
    QVariantList longitudes;
    QVariantList latitudes;
    QVariantList altitudes;
    QVariantList speeds;
    QVariantList refRaces;
    QVariantList refNums;
    QVector<GeoCoordinate> coords;

    int frameCount(0);
    int nbInterval(0);
    bool inInterval(false);
    QTime startCollectTime;
    QTime endCollectTime;

    while(!in.atEnd())
    {
        QString line = in.readLine();
        GeoCoordinate coord(line);

        if (coord.goodtype() && coord.valid())
        {
            coords << coord;

            if (!inInterval)
            {
                nbInterval ++;
                qDebug() << frameCount;
                inInterval = true;
            }
            frameCount++;
        }
        else
        {
            qDebug() << "frame not valid : " << line;
            if (coord.goodtype())
            {
                frameCount++;
                inInterval = false;
            }
        }

        if (frameCount == 1)
            startCollectTime = coord.time();
        endCollectTime = coord.time();
    }

    qDebug() << "----> " << nbInterval << " intervals";
    qDebug() << "----> " << (coords.size() * 100.0) / frameCount;

    QList< QPair<QTime, QTime> > laps;

    if (nbInterval > 6 || (coords.size() * 100.0 / frameCount) < 65)
    {
        qDebug() << "[!] Detection skipped";
        laps << QPair<QTime, QTime>(startCollectTime, endCollectTime);
    }
    else
    {
        RaceViewer viewer(coords.toList());

        if(viewer.exec() != QDialog::Accepted)
            throw QException(tr("Chargement de la course annulé"));

        laps = viewer.laps();

//        LapDetector ld(&coords);
//        laps = ld.laps();

        if (laps.size() == 0)
        {
            qDebug() << "[!] no laps founds, creating global one.";
            laps << QPair<QTime, QTime>(startCollectTime, endCollectTime);
        }

    }

    int nbcoord = coords.size();
    int raceId = race.id();
    int j = 0;

    for (int i = 0; i < laps.size(); ++i)
    {
        qDebug()  << laps[i].first.toString() << " " << laps[i].second.toString();
        QTime start = laps[i].first;
        QTime end = laps[i].second;

        /*
         * Dans le cas ou l'on aurait des donnees gps alteres et ou la detection de tour serait impossible
         * sur cette base, il faudrait tout de meme charger les donnees vitesses et acceleration pour
         * conserver la visualisation brut des donnees

         * Surement de sages paroles. TODO
         */
        this->createLap(race, i, start, end);

        /* Filtrage de la zone pre-course (zone stand, ...)*/
        while (i == 0 && j < nbcoord && coords[j].time() < start)
        {
//            qDebug() << "skipping " << j;
            j++;
        }

        while (j < nbcoord && coords[j].time() <= end)
        {
            //                qDebug() << coords[j].time() << start << start.msecsTo(coords[j].time());
            timestamps << start.msecsTo(coords[j].time());
            latitudes << coords[j].latitude();
            longitudes << coords[j].longitude();
            altitudes << coords[j].altitude();
            speeds << coords[j].speed();
            refRaces << raceId;
            refNums << i;
            j++;
        }
    }

    query.addBindValue(timestamps);
    query.addBindValue(longitudes);
    query.addBindValue(latitudes);
    query.addBindValue(altitudes);
    query.addBindValue(speeds);
    query.addBindValue(refRaces);
    query.addBindValue(refNums);

    DataBaseManager::execBatch(query, QSqlQuery::ValuesAsColumns);
}

void DataBaseImportModule::loadSpeedData(
        const QString &speedFilePath, Race& race)
{
    qDebug() << "Chargement des données de vitesse ...";

    QFile speedFile(speedFilePath);
    if (!speedFile.open(QIODevice::ReadOnly))
        throw QException(tr("Impossible d'ouvrir le fichier ") + speedFilePath);

    QSqlQuery query;
    query.prepare("insert into SPEED (timestamp, value, ref_lap_race, ref_lap_num) values (?, ?, ?, ?)");
    QVariantList timestamps;
    QVariantList values;
    QVariantList refRaces;
    QVariantList refNums;
    int raceId = race.id();
    int prevNumLap = -2;
    int numLap;

    qint64 prevAbsTime;
    qint64 absTime;
    qint64 origin;

    QTextStream in(&speedFile);
    if (in.atEnd())
        throw QException(tr("le fichier ") + speedFilePath + tr(" est vide"));

    /* ---------------------------------------------------------------------- *
     *       Lecture de la première valeur de temps comme (origine)           *
     *                        temps du début du tour                          *
     * ---------------------------------------------------------------------- */
    QString line = in.readLine();

    origin = line.section(";", 0, 0).toULongLong() * 1000000000
           + (line.section(";", -1, -1).toULongLong());

    prevAbsTime = origin;
    QTime lapTimeOrigin(0, 0);

    qDebug() << "Périmètre de la roue : " << race.wheelPerimeter();
    race.restartLapCounting();
    while (!in.atEnd())
    {
        QString currentLine = in.readLine();
        absTime = currentLine.section(";", 0, 0).toULongLong() * 1000000000
                + (currentLine.section(";", -1, -1).toULongLong()); // Lecture de la deuxième à la dernière ligne
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(absTime / (1000 * 1000)); // Toutes les données de temps sont expirmiées en millisecondes depuis l'époque
        numLap = race.numLap(dt.time());

        // Si on change de tour
        if (numLap != prevNumLap)
        {
            origin = absTime; // Le temps de référence pour le début du tour = à la donnée qui vient d'etre lue
            prevNumLap = numLap;
            lapTimeOrigin = race.lap(numLap).first;
        }
//        qDebug() << absTime << " - " << origin << " = " << absTime - origin;

        //FIXME
        Q_ASSERT((absTime - prevAbsTime) >= 0);
        qreal value = race.wheelPerimeter() * 3600.0 * 1000 * 1000 / (absTime - prevAbsTime); // vitesse en km/h

        if (numLap == -1)
            continue;

        // FIXME : filter max value
        if (!qIsInf(value) && value < 80) {
            timestamps << lapTimeOrigin.msecsTo(dt.time());
            values << value;
            refRaces << raceId;
            refNums << numLap;
        }

        prevAbsTime = absTime;
    }

    query.addBindValue(timestamps);
    query.addBindValue(values);
    query.addBindValue(refRaces);
    query.addBindValue(refNums);

    DataBaseManager::execBatch(query, QSqlQuery::ValuesAsColumns);
}

void DataBaseImportModule::loadSpeedData_2(
        const QString &speedFilePath, Race &race)
{
    qDebug() << "Chargement des données de vitesse, distance, accélération ...";

    /* ---------------------------------------------------------------------- *
     *                Vérifie la validité du fichier de données               *
     * ---------------------------------------------------------------------- */

    QFile speedFile(speedFilePath);
    if (!speedFile.open(QIODevice::ReadOnly))
        throw QException(tr("Impossible d'ouvrir le fichier ") + speedFilePath);

    QTextStream in(&speedFile);
    if (in.atEnd())
        throw QException(tr("le fichier ") + speedFilePath + tr(" est vide"));

    /* ---------------------------------------------------------------------- *
     *                     Requete d'insertion des données                    *
     * ---------------------------------------------------------------------- */

    QSqlQuery query("INSERT INTO datarace VALUES (?, ?, ?, ?, ?, ?)");
    QVariantList timestamps;
    QVariantList refNums;
    QVariantList refRaces;
    QVariantList speeds;
    QVariantList distances;
    QVariantList accelerations;

    /* ---------------------------------------------------------------------- *
     *     Boucle de calcul des données de vitesse, distance, accélération    *
     * ---------------------------------------------------------------------- */

    const int raceId(race.id());
    const qreal wheelPerimeter(race.wheelPerimeter());

    int currentLap(-1),  previousLap(-1);
    quint64 currentTime(0), previousTime(0);
    qreal   currentSpeed(0), previousSpeed(0);
    qreal   currentPos(wheelPerimeter), previousPos(0);
    QTime lapTimeOrigin(0, 0);
    QString currentLine;

    // Lecture de la première données de temps (t1)
    currentLine = in.readLine();
    previousTime = currentLine.section(";", 0, 0).toULongLong() * 1000000000
                 + (currentLine.section(";", -1, -1).toULongLong());

    // Lecture de la deuxième données de temps (t2)
    currentLine = in.readLine();
    currentTime = currentLine.section(";", 0, 0).toULongLong() * 1000000000
                + (currentLine.section(";", -1, -1).toULongLong());

    // Calcul de la première vitesse (sera previousSpeed une fois ds la boucle)
    currentSpeed = wheelPerimeter * 3600.0 * 1000 * 1000
                                         /
                            (currentTime - previousTime);

    race.restartLapCounting();
    while(!in.atEnd())
    {
        previousTime  = currentTime;
        previousSpeed = currentSpeed;
        previousPos   = currentPos;

        // Lecture du nombre de secondes + nanosecondes écoulées depuis l'époque
        currentLine = in.readLine();
        currentTime = currentLine.section(";", 0, 0).toULongLong() * 1000000000
                    + (currentLine.section(";", -1, -1).toULongLong());

        // Détermination de l'appartenance à un tour
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(currentTime / (1000 * 1000));
        currentLap = race.numLap(dt.time());

        // Données qui précèdent l'heure de début du premier tour
        if (currentLap < 0)
        {
            qDebug() << "données avant début du tour ...";
            continue;
        }

        // Si on est passé au tour suivant
        if (currentLap > previousLap)
        {
            previousLap = currentLap;
            lapTimeOrigin = race.lap(currentLap).first;
            previousPos = 0;
        }

        /* ------------------------------------------------------------------ *
         *                    Calcul de la vitesse en km/h                    *
         * ------------------------------------------------------------------ */

        currentSpeed = wheelPerimeter * 3600.0 * 1000 * 1000
                                             /
                                (currentTime - previousTime);

        /* Si on a une vitesse supérieure à 70 km/h, c-à-d si deux captures de
         * temps sont trop proches, on ignore cette donnée */
        if (currentSpeed > 70)
            continue;

        /* ------------------------------------------------------------------ *
         *                     Calcul de la distance en m                     *
         * ------------------------------------------------------------------ */

        double deltaTimeSeconds = (currentTime - previousTime) * 0.000000001;

        int multipleWheelPerimeter = ceil(((currentSpeed + previousSpeed) / (2 * 3.6)) * deltaTimeSeconds) / wheelPerimeter;

        if (multipleWheelPerimeter < 10)
            currentPos = previousPos + multipleWheelPerimeter * wheelPerimeter;

        /* ------------------------------------------------------------------ *
         *                  Calcul de l'accélération en m/s²                  *
         * ------------------------------------------------------------------ */

        qreal diff = (currentSpeed - previousSpeed) / 3.6; // Vitesse en m/s
        qreal acc  = diff / deltaTimeSeconds;

        /* ------------------------------------------------------------------ *
         *                     Ajout des données du tuple                     *
         * ------------------------------------------------------------------ */

        timestamps << lapTimeOrigin.msecsTo(dt.time());
        refNums << currentLap;
        refRaces << raceId;
        speeds << currentSpeed;
        distances << currentPos;
        accelerations << acc;
    }

    /* ---------------------------------------------------------------------- *
     *              Insertion des données dans la base de données             *
     * ---------------------------------------------------------------------- */

    query.addBindValue(timestamps);
    query.addBindValue(refNums);
    query.addBindValue(refRaces);
    query.addBindValue(speeds);
    query.addBindValue(distances);
    query.addBindValue(accelerations);

    DataBaseManager::execBatch(query, QSqlQuery::ValuesAsColumns);
}

void DataBaseImportModule::loadMegasquirtData(
        const QString &megasquirtDATFilePath,
        const QString &megasquirtCSVFilePath,
        Race &race)
{
    qDebug() << "Chargement des données Megasquirt...";

    /* ---------------------------------------------------------------------- *
     *                     Create csv file from dat file                      *
     * ---------------------------------------------------------------------- */

    // Remove oldest megasquirt csv file if exists
    QFile msCSVFile(megasquirtCSVFilePath);
    if (msCSVFile.exists())
        msCSVFile.remove();

    // Create parser
    QCSVParser parser(megasquirtCSVFilePath, ';');

    // Conversion des données Megasquirt
    MSManager manager;
    manager.datToCSV(megasquirtDATFilePath, parser, manager.fields());

    // Save data
    parser.save();

    /* ---------------------------------------------------------------------- *
     *                     add megasquirt data in database                    *
     * ---------------------------------------------------------------------- */

    QSqlQuery query;
    query.prepare("insert into MEGASQUIRT values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    QVariantList timestamps;
    QVariantList refRaces;
    QVariantList refNums;

    QList<QVariantList> columns;

    qint64 nsSinceEpoch;
    QDateTime dt;
    int raceId(race.id());
    int prevNumLap(-2);
    int numLap;
    QTime lapTimeOrigin(0, 0);
    int toursRefuses(0);

    /* ---------------------------------------------------------------------- *
     *  Boucle de calcul pour le timestamp et la référence du numéro du tour  *
     * ---------------------------------------------------------------------- */

    race.restartLapCounting();
    for(int rowNum(1); rowNum < parser.rowCount(); ++rowNum)
    {
        nsSinceEpoch = parser.row(rowNum).at(0).toULongLong() * 1000000000
                     + parser.row(rowNum).at(1).toULongLong();
        dt = QDateTime::fromMSecsSinceEpoch(nsSinceEpoch / (1000 * 1000));

        //qDebug() << dt;

        numLap = race.numLap(dt.time());

        if(numLap == -1)
        {
            qDebug() << "tour refusé !!!!!";
            ++toursRefuses;
            continue;
        }

        // Si on change de tour
        if(numLap != prevNumLap)
        {
            prevNumLap = numLap;
            lapTimeOrigin = race.lap(numLap).first;
        }

        timestamps << lapTimeOrigin.msecsTo(dt.time());
        refRaces   << raceId;
        refNums    << numLap;
    }

    columns << timestamps << refNums << refRaces;

    /* ---------------------------------------------------------------------- *
     *                   Récupération des données Megasquirt                  *
     * ---------------------------------------------------------------------- */

    for(int i(2); i < parser.columnCount(); ++i) // Saute les sec et nanosec
        columns << DataBaseManager::toVariantList(parser.column(i).mid(toursRefuses));

    foreach (QVariantList column, columns)
        query.addBindValue(column);

    DataBaseManager::execBatch(query, QSqlQuery::ValuesAsColumns);
}
