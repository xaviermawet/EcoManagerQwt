#include "QCSVParser.hpp"

QCSVParser::QCSVParser(const QString &filename, const QChar &separator,
                       QString::SplitBehavior behavior) :
    _fileName(filename), _separator(separator), _behavior(behavior)
{
    // Parse the file
    this->parse(filename, separator, behavior);
}

QCSVParser::QCSVParser(QFile &file, const QChar &separator,
                       QString::SplitBehavior behavior) :
    _fileName(file.fileName()), _separator(separator), _behavior(behavior)
{
    // Parse the file
    if (file.exists())
        this->parse(file, separator, behavior);
}

QCSVParser::~QCSVParser(void)
{
    qDebug() << "QCSVParser début destructeur";

    // Nothing to do...

    qDebug() << "QCSVParser fin destructeur";
}

void QCSVParser::parse(const QString &filename, const QChar &separator,
                       QString::SplitBehavior behavior)
{
    // Delegate parsing
    QFile file(filename);
    if (file.exists())
        this->parse(file, separator, behavior);
}

void QCSVParser::parse(QFile &file, const QChar &separator,
                       QString::SplitBehavior behavior)
{
    /* Try to open the file
     * The QIODevice::Text flag passed to open() tells Qt to convert Windows-
     * style line terminators ("\r\n") into C++-style terminators ("\n")
     */
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw QException(QObject::tr("Impossible d'ouvrir le fichier ")
                         + file.fileName());

    // Open succed
    this->_fileName  = file.fileName();
    this->_separator = separator;
    this->_behavior  = behavior;
    this->_content.clear();

    // Read file line by line
    QString line;
    while(!file.atEnd())
    {
        line = file.readLine();
        this->_content.push_back(
                    line.split(this->_separator, this->_behavior).toVector());
    }

    file.close();
}

void QCSVParser::save(void) const
{
    QFile file(this->_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        throw QException(QObject::tr("Impossible d'ouvrir le fichier"));

    // Write all the line
    QTextStream out(&file);
    foreach (QCSVRow row, this->_content)
        this->writeRow(out, row);

    file.close();
}

int QCSVParser::rowCount(void) const
{
    return this->_content.count();
}

int QCSVParser::columnCount(void) const
{
    return this->header().count();
}

QCSVRow QCSVParser::header(void) const
{
    return this->_content.value(0, QCSVRow());
}

QStringList QCSVParser::headersList(void) const
{
    return QStringList(this->header().toList());
}

QCSVRow& QCSVParser::row(int index)
{
    if (index >= this->rowCount())
        throw QException(QObject::tr("Indice trop grand"));

    return this->_content[index];
}

QCSVColumn& QCSVParser::column(int index)
{
    if (index >= this->columnCount())
        throw QException(QObject::tr("Indice trop grand"));

    this->_column.clear();

    for (int i(1); i < this->rowCount(); ++i) // skeep the header
        this->_column.push_back(this->_content[i][index]);

    return this->_column;

    /*
    QCSVColumn col;
    foreach (QCSVRow row, this->_content)
        col.push_back(row.at(index));

    return col;
    */
}

void QCSVParser::addRow(const QCSVRow& row)
{
    /*
    QFile file(this->_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        throw QException(QObject::tr("Impossible d'ouvrir le fichier"));

    // Append the line
    QTextStream out(&file);
    this->writeRow(out, row);

    file.close();
    */

    // Add row the the csv content
    this->_content.push_back(row);
}

void QCSVParser::eraseRow(int rowindex)
{
    this->_content.remove(rowindex);

    //this->save();
}

void QCSVParser::reset(void)
{
    this->_fileName  = QString();
    this->_separator = QChar();
    this->_behavior  = DEFAULT_BEHAVIOR;

    // Clear the DB
    this->_content.clear();
    this->_column.clear();
}

QString QCSVParser::fileName(void) const
{
    return this->_fileName;
}

QChar QCSVParser::separator(void) const
{
    return this->_separator;
}

QString::SplitBehavior QCSVParser::behavior(void) const
{
    return this->_behavior;
}

QString QCSVParser::columnName(int column) const
{
    if (column >= this->columnCount())
        throw QException(QObject::tr("Indice trop grand"));

    return this->header().at(column);
}

int QCSVParser::columnIndice(const QString& columnName) const
{
    QCSVRow header = this->header();
    for (int i(0); i < header.count(); ++i)
        if (header.at(i) == columnName)
            return i;

    throw QException(QObject::tr("Column not found"));
}

QCSVColumn& QCSVParser::operator [](QString const& columName)
{
    return this->column(this->columnIndice(columName));
}

QCSVColumn& QCSVParser::operator [](int column)
{
    return this->column(column);
}

void QCSVParser::writeRow(QTextStream& out, const QCSVRow& row) const
{
    for (int i(0); i < row.count() - 1; ++i)
        out << row.at(i) << this->_separator;
    out << row.at(row.count() - 1) << endl;

    /*
    for (int i(0); i < row.count(); ++i)
        out << row.at(i) << this->_separator;
    out << endl;
    */
}
