#include "MSManager.hpp"

MSManager::MSManager(void)
{
    this->dataConverters
    << MSDataConverter("seconds",       MSDataConverter::U16, 0, "s", 1.000, 0.0)
    << MSDataConverter("pulseWidth1",   MSDataConverter::U16, 2, "s", 0.000666, 0.0)
    << MSDataConverter("pulseWidth2",   MSDataConverter::U16, 4, "s", 0.000666, 0.0)//
    << MSDataConverter("rpm",           MSDataConverter::U16, 6, "RPM", 1.000, 0.0)
    << MSDataConverter("advance",       MSDataConverter::S16, 8, "deg", 0.100, 0.0)
    << MSDataConverter("squirt",        MSDataConverter::U08, 10, "bit", 1.000, 0.0)
    << MSDataConverter("engine",        MSDataConverter::U08, 11, "bit", 1.000, 0.0)
    << MSDataConverter("afrtgt1",       MSDataConverter::U08,  	 12, "AFR",		0.1,				0.0)
    << MSDataConverter("afrtgt2",       MSDataConverter::U08,  	 13, "AFR",		0.1, 				0.0)//
    << MSDataConverter("wbo2_en1",      MSDataConverter::U08,  	 14, "",		1.000, 				0.0)//
    << MSDataConverter("wbo2_en2",      MSDataConverter::U08,  	 15, "",		1.000, 				0.0)//
    << MSDataConverter("barometer",     MSDataConverter::S16,  	 16, "kPa", 	0.100, 				0.0)
    << MSDataConverter("map",           MSDataConverter::S16,  	 18, "kPa", 	0.100, 				0.0)
    << MSDataConverter("mat",           MSDataConverter::S16,  	 20, "B0C", 	0.05555, 			-320.0)
    << MSDataConverter("coolant",       MSDataConverter::S16,  	 22, "B0C", 	0.05555, 			-320.0)
    << MSDataConverter("tps",           MSDataConverter::S16,  	 24, "%",		0.100, 				0.0)
    << MSDataConverter("batteryVoltage",MSDataConverter::S16,  	 26, "v",		0.100, 				0.0)
    << MSDataConverter("afr1",          MSDataConverter::S16,  	 28, "AFR", 	0.100, 				0.0)
    << MSDataConverter("afr2",          MSDataConverter::S16,  	 30, "AFR", 	0.100, 				0.0)//
    << MSDataConverter("knock",         MSDataConverter::S16,  	 32, "v",		0.100, 				0.0)//
    << MSDataConverter("egoCorrection1",MSDataConverter::S16,  	 34, "%",		0.1000, 			0.0)
    << MSDataConverter("egoCorrection2",MSDataConverter::S16,  	 36, "%",		0.1000, 			0.0)//
    << MSDataConverter("airCorrection", MSDataConverter::S16,  	 38, "%",		1.000, 				0.0)
    << MSDataConverter("warmupEnrich",  MSDataConverter::S16,  	 40, "%",		1.000, 				0.0)
    << MSDataConverter("accelEnrich",   MSDataConverter::S16,  	 42, "ms", 		0.100, 				0.0)
    << MSDataConverter("tpsfuelcut",    MSDataConverter::S16,  	 44, "%",		1.000, 				0.0)
    << MSDataConverter("baroCorrection",MSDataConverter::S16,  	 46, "%",		1.000, 				0.0)
    << MSDataConverter("gammaEnrich",   MSDataConverter::S16,  	 48, "%",		1.000, 				0.0)
    << MSDataConverter("veCurr1",       MSDataConverter::S16,  	 50, "%",		0.1000, 			0.0)
    << MSDataConverter("veCurr2",       MSDataConverter::S16,  	 52, "%",		0.1000, 			0.0)//
    << MSDataConverter("iacstep",       MSDataConverter::S16,  	 54, "",		1.000, 				0.0)//
    << MSDataConverter("idleDC",        MSDataConverter::S16,  	 54, "",		0.39063, 			0.0)//
    << MSDataConverter("coldAdvDeg",    MSDataConverter::S16,  	 56, "deg", 	0.100, 				0.0)
    << MSDataConverter("tpsDOT",        MSDataConverter::S16,  	 58, "%/s",		0.100, 				0.0)
    << MSDataConverter("mapDOT",        MSDataConverter::S16,  	 60, "kPa/s", 	1.000, 				0.0)
    << MSDataConverter("dwell",         MSDataConverter::U16,  	 62, "ms",		0.0666, 			0.0)
    << MSDataConverter("mafmap",        MSDataConverter::S16,  	 64, "kPa", 	0.100, 				0.0)
    << MSDataConverter("fuelload",      MSDataConverter::S16,  	 66, "%",		0.100, 				0.0)
    << MSDataConverter("fuelCorrection",MSDataConverter::S16,  	 68, "%", 		1.000, 				0.0)
    << MSDataConverter("portStatus",    MSDataConverter::U08,  	 70, "bit",		1.000, 				0.0)
    << MSDataConverter("knockRetard",   MSDataConverter::U08,  	 71, "deg", 	0.1, 				0.0)//
    << MSDataConverter("EAEFuelCorr1",  MSDataConverter::U16,  	 72, "%",  		1.0, 				0.0)
    << MSDataConverter("egoV",          MSDataConverter::S16,  	 74, "V", 		0.01, 				0.0)
    << MSDataConverter("egoV2",         MSDataConverter::S16,  	 76, "V", 		0.01, 				0.0)//
    << MSDataConverter("status1",       MSDataConverter::U08,  	 78, "", 		1.0, 				0.0)//
    << MSDataConverter("status2",       MSDataConverter::U08,  	 79, "", 		1.0, 				0.0)//
    << MSDataConverter("status3",       MSDataConverter::U08,  	 80, "", 		1.0, 				0.0)//
    << MSDataConverter("status4",       MSDataConverter::U08,  	 81, "", 		1.0, 				0.0)//
    << MSDataConverter("looptime",      MSDataConverter::U16,  	 82, "us", 		0.6667, 			0.0)//
    << MSDataConverter("status5",       MSDataConverter::U16,  	 84, "us", 		1.0, 				0.)//
    << MSDataConverter("tpsADC",        MSDataConverter::U16,  	 86, "ADC", 	1.0, 				0.0)
    << MSDataConverter("fuelload2",     MSDataConverter::S16,  	 88, "%",		0.100, 				0.0)//
    << MSDataConverter("ignload",       MSDataConverter::S16,  	 90, "%",		0.100, 				0.0)
    << MSDataConverter("ignload2",      MSDataConverter::S16,  	 92, "%",		0.100, 				0.0)//
    << MSDataConverter("synccnt",       MSDataConverter::U08,  	 94, "", 		1.0, 				0.0)
    << MSDataConverter("timing_err",    MSDataConverter::S08,  	 95, "%", 		0.1, 				0.0)
    << MSDataConverter("deltaT",        MSDataConverter::S32,  	 96, "uS",		1.000, 				0.0)
    << MSDataConverter("wallfuel1",     MSDataConverter::U32,  	100, "uS",		1.000, 				0.0)
    << MSDataConverter("gpioadc0",      MSDataConverter::U16,  	104, "",		1.000, 				0.0)//
    << MSDataConverter("gpioadc1",      MSDataConverter::U16,  	106, "",		1.000, 				0.0)//
    << MSDataConverter("gpioadc2",      MSDataConverter::U16,  	108, "",		1.000, 				0.0)//
    << MSDataConverter("gpioadc3",      MSDataConverter::U16,  	110, "",		1.000, 				0.0)//
    << MSDataConverter("gpioadc4",      MSDataConverter::U16,  	112, "",		1.000, 				0.0)//
    << MSDataConverter("gpioadc5",      MSDataConverter::U16,  	114, "",		1.000, 				0.0)//
    << MSDataConverter("gpioadc6",      MSDataConverter::U16,  	116, "",		1.000, 				0.0)//
    << MSDataConverter("gpioadc7",      MSDataConverter::U16,  	118, "",		1.000, 				0.0)//
    << MSDataConverter("gpiopwmin0",    MSDataConverter::U16,  	120, "",		1.000, 				0.0)//
    << MSDataConverter("gpiopwmin1",    MSDataConverter::U16,  	122, "",		1.000, 				0.0)//
    << MSDataConverter("gpiopwmin2",    MSDataConverter::U16,  	124, "",		1.000, 				0.0)//
    << MSDataConverter("gpiopwmin3",    MSDataConverter::U16,  	126, "",		1.000, 				0.0)//
    << MSDataConverter("adc6",          MSDataConverter::U16,  	128, "",		1.0, 				0.0)//
    << MSDataConverter("adc7",          MSDataConverter::U16,  	130, "",		1.0, 				0.0)//
    << MSDataConverter("wallfuel2",     MSDataConverter::U32,  	132, "uS", 		1.000, 				0.0)//
    << MSDataConverter("EAEFuelCorr2",  MSDataConverter::U16,  	136, "%",  		1.0, 				0.0)//
    << MSDataConverter("boostduty",     MSDataConverter::U08,  	138, "%",  		1.0, 				0.0)
    << MSDataConverter("syncreason",    MSDataConverter::U08,  	139, "", 		1.0, 				0.0)
    << MSDataConverter("user0",         MSDataConverter::U16,  	140, "", 		1.0, 				0.0)//
    << MSDataConverter("inj_adv1",      MSDataConverter::S16,  	142, "deg", 	0.100, 				0.0)
    << MSDataConverter("inj_adv2",      MSDataConverter::S16,  	144, "deg", 	0.100, 				0.0)//
    << MSDataConverter("pulseWidth3",   MSDataConverter::U16,  	146, "s",		0.000666, 			0.0)//
    << MSDataConverter("pulseWidth4",   MSDataConverter::U16,  	148, "s",		0.000666, 			0.0)//
    << MSDataConverter("vetrim1curr",   MSDataConverter::S16,  	150, "%",		0.00976562500,      10240.0)
    << MSDataConverter("vetrim2curr",   MSDataConverter::S16,  	152, "%",		0.00976562500,      10240.0)//
    << MSDataConverter("vetrim3curr",   MSDataConverter::S16,  	154, "%",		0.00976562500,      10240.0)//
    << MSDataConverter("vetrim4curr",   MSDataConverter::S16,  	156, "%",		0.00976562500,      10240.0)//
    << MSDataConverter("maf",           MSDataConverter::U16,  	158, "g/sec", 	0.010,  			0.0)
    << MSDataConverter("eaeload1",      MSDataConverter::S16,  	160, "%", 		0.1, 				0.0)
    << MSDataConverter("afrload1",      MSDataConverter::S16,  	162, "%", 		0.1, 				0.0)
    << MSDataConverter("rpmdot",        MSDataConverter::S16,  	164, "rpm/sec", 10.0, 				0.0)
    << MSDataConverter("gpioport0",     MSDataConverter::U08,  	166, "",		1.000, 				0.0)//
    << MSDataConverter("gpioport1",     MSDataConverter::U08,  	167, "",		1.000, 				0.0)//
    << MSDataConverter("gpioport2",     MSDataConverter::U08,  	168, "",		1.000, 				0.0);//
}

int MSManager::count(void) const
{
    return this->dataConverters.count();
}

int MSManager::fieldIndice(const QString& field) const
{
    for (int i(0); i < this->count(); ++i)
        if (this->dataConverters.at(i).field() == field)
            return i;

    throw QException(QObject::tr("Field not found"));
}

QStringList MSManager::fields(void) const
{
    QStringList megasquirtFields;
    foreach (MSDataConverter converter, this->dataConverters)
        megasquirtFields << converter.field();

    return megasquirtFields;
}

const MSDataConverter& MSManager::dataConverter(int fieldIndice) const
{
    if (fieldIndice >= this->count())
        throw QException(QObject::tr("Indice trop grand"));

    return this->dataConverters.at(fieldIndice);
}

const MSDataConverter& MSManager::dataConverter(const QString& field) const
{
    //return this->dataConverters.at(this->fieldIndice(field));

    for (int i(0); i < this->count(); ++i)
        if (this->dataConverters.at(i).field() == field)
            return this->dataConverters.at(i);

    throw QException(QObject::tr("Field not found"));
}

const MSDataConverter& MSManager::operator [](QString const& field) const
{
    return this->dataConverter(field);
}

const MSDataConverter& MSManager::operator [](int fieldIndice) const
{
    return this->dataConverter(fieldIndice);
}

void MSManager::datToCSV(const QString &datFile, const QString &CSVFile,
                         const QStringList &fields) const
{
    // Remove oldest megasquirt csv file if exists
    QFile msCSVFile(CSVFile);
    if (msCSVFile.exists())
        msCSVFile.remove();

    // Create parser
    QCSVParser parser(CSVFile, ';');

    // Get data
    this->datToCSV(datFile, parser, fields);

    // Save data
    parser.save();
}

void MSManager::datToCSV(const QString &datFile, QCSVParser &parser,
                         const QStringList &fields) const
{
    // File *.dat : read binaries data from a stream
    QFile fileDAT(datFile);
    if (!fileDAT.open(QIODevice::ReadOnly))
        throw QException(QObject::tr("Impossible d'ouvrir le fichier") +
                         datFile);

    // Bytes order is QDataStream::BigEndian by default
    QDataStream in(&fileDAT);

    QCSVRow row;
    quint32 secondsSinceEpoch, nanoSeconds;
    unsigned char buffer[DATA_SIZE];
    unsigned long  uvar;
    signed   long  svar;
    double         value;

    /* Add CSV headers */
    row << "secondsSinceEpoch" << "nanoSeconds";
    row << fields.toVector();
    parser.addRow(row);

    do
    {
        row.clear();

        // the number of seconds since epoch
        in >> secondsSinceEpoch;
        row << QString::number(qFromBigEndian<quint32>(secondsSinceEpoch));

        // the number of nanoseconds to add
        in >> nanoSeconds;
        row << QString::number(qFromBigEndian<quint32>(nanoSeconds));

        // Megasquirt data conversion
        if (in.readRawData((char*)buffer, DATA_SIZE) < DATA_SIZE)
            throw QException(QObject::tr("Trame Megasquirt de taille invalide"));

        foreach (QString field, fields)
        {
            MSDataConverter dataconvert = this->dataConverter(field);
            uvar = svar = 0;
            value = 0.0;

            /* data extraction from buffer */
            switch (dataconvert.type())
            {
                case MSDataConverter::U08:
                    memcpy(&uvar, buffer + dataconvert.offset(), 1);
                    value = (double)uvar;
                    break;
                case MSDataConverter::U16:
                    memcpy(&uvar, buffer + dataconvert.offset(), 2);
                    uvar  = qFromBigEndian<quint16>((uchar*)&uvar); // MegaSquirt is big endian
                    value = (double)uvar;
                    break;
                case MSDataConverter::U32:
                    memcpy(&uvar, buffer + dataconvert.offset(), 4);
                    uvar  = qFromBigEndian<quint32>((uchar*)&uvar); // MegaSquirt is big endian
                    value = (double)uvar;
                    break;
                case MSDataConverter::S08:
                    memcpy(&svar, buffer + dataconvert.offset(), 1);
                    value = (double)svar;
                    break;
                case MSDataConverter::S16:
                    memcpy(&svar, buffer + dataconvert.offset(), 2);
                    svar  = qFromBigEndian<quint16>((uchar*)&svar); // MegaSquirt is big endian
                    value = (double)svar;
                    break;
                case MSDataConverter::S32:
                    memcpy(&svar, buffer + dataconvert.offset(), 4);
                    svar  = qFromBigEndian<quint32>((uchar*)&svar); // MegaSquirt is big endian
                    value = (double)svar;
                    break;
            }

            /* data convertion */
            value = (value + dataconvert.translate()) * dataconvert.scale();

            /* add value to the row */
            row << QString::number(value, 'f');
        }

        parser.addRow(row);

    } while (!fileDAT.atEnd());

    fileDAT.close();
}

//void MSManager::datToCSV(const QString& datFile, const QString& CSVFile,
//                         const QStringList& fields) const
//{
//    // File *.dat : read binaries data from a stream
//    QFile fileDAT(datFile);
//    if (!fileDAT.open(QIODevice::ReadOnly))
//        throw QException(QObject::tr("Impossible d'ouvrir le fichier") +
//                         datFile);

//    // Bytes order is QDataStream::BigEndian by default
//    QDataStream in(&fileDAT);

//    // Remove oldest megasquirt csv file if exists
//    QFile msCSVFile(CSVFile);
//    if (msCSVFile.exists())
//        msCSVFile.remove();

//    QCSVParser parser(CSVFile, ';');
//    QCSVRow row;

//    quint32 secondsSinceEpoch, nanoSeconds;
//    unsigned char buffer[DATA_SIZE];
//    unsigned long  uvar;
//    signed   long  svar;
//    double         value;

//    /* Add CSV headers */
//    row << "secondsSinceEpoch" << "nanoSeconds";
//    row << fields.toVector();
//    parser.addRow(row);

//    do
//    {
//        row.clear();

//        // Secondes depuis l'époque
//        in >> secondsSinceEpoch;
//        row << QString::number(qFromBigEndian<quint32>(secondsSinceEpoch));

//        // Nano secondes à ajouter aux secondes
//        in >> nanoSeconds;
//        row << QString::number(qFromBigEndian<quint32>(nanoSeconds));

//        // Conversion des données Megasquirt
//        if (in.readRawData((char*)buffer, DATA_SIZE) < DATA_SIZE)
//            throw QException(QObject::tr("Trame Megasquirt de taille invalide"));

//        foreach (QString field, fields)
//        {
//            MSDataConverter dataconvert = this->dataConverter(field);
//            uvar = svar = 0;
//            value = 0.0;

//            /* data extraction from buffer */
//            switch (dataconvert.type())
//            {
//                case MSDataConverter::U08:
//                    memcpy(&uvar, buffer + dataconvert.offset(), 1);
//                    value = (double)uvar;
//                    break;
//                case MSDataConverter::U16:
//                    memcpy(&uvar, buffer + dataconvert.offset(), 2);
//                    uvar  = qFromBigEndian<quint16>((uchar*)&uvar); // MegaSquirt is big endian
//                    value = (double)uvar;
//                    break;
//                case MSDataConverter::U32:
//                    memcpy(&uvar, buffer + dataconvert.offset(), 4);
//                    uvar  = qFromBigEndian<quint32>((uchar*)&uvar); // MegaSquirt is big endian
//                    value = (double)uvar;
//                    break;
//                case MSDataConverter::S08:
//                    memcpy(&svar, buffer + dataconvert.offset(), 1);
//                    value = (double)svar;
//                    break;
//                case MSDataConverter::S16:
//                    memcpy(&svar, buffer + dataconvert.offset(), 2);
//                    svar  = qFromBigEndian<quint16>((uchar*)&svar); // MegaSquirt is big endian
//                    value = (double)svar;
//                    break;
//                case MSDataConverter::S32:
//                    memcpy(&svar, buffer + dataconvert.offset(), 4);
//                    svar  = qFromBigEndian<quint32>((uchar*)&svar); // MegaSquirt is big endian
//                    value = (double)svar;
//                    break;
//            }

//            /* data convertion */
//            value = (value + dataconvert.translate()) * dataconvert.scale();

//            /* add value to the row */
//            row << QString::number(value, 'f');
//        }

//        parser.addRow(row);

//    } while (!fileDAT.atEnd());

//    // Sauvegarder le csv
//    parser.save();
//}
