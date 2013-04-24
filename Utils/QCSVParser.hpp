#ifndef __QCSVPARSER_HPP__
#define __QCSVPARSER_HPP__

#if QT_VERSION >= 0x050000 //  0xMMNNPP (MM = major, NN = minor, PP = patch)
    #include <QtWidgets>
#else
    #include <QtGui>
#endif

#include "QException.hpp"

#define DEFAULT_BEHAVIOR QString::KeepEmptyParts

// TYPE DEFINITIONS
typedef QVector<QString> QCSVRow;
typedef QVector<QString> QCSVColumn;
typedef QVector<QCSVRow> QCSVDataBase;

class QCSVParser
{
    public:

        QCSVParser(void);
        explicit QCSVParser(const QString& filename,
                            const QChar& separator = ',',
                            QString::SplitBehavior behavior = DEFAULT_BEHAVIOR);
        explicit QCSVParser(QFile& file,
                            const QChar& separator = ',',
                            QString::SplitBehavior behavior = DEFAULT_BEHAVIOR);
        ~QCSVParser(void);

        void parse(const QString& filename, const QChar& separator = ',',
                   QString::SplitBehavior behavior = DEFAULT_BEHAVIOR);
        void parse(QFile& file, const QChar& separator = ',',
                   QString::SplitBehavior behavior = DEFAULT_BEHAVIOR);
        void save(void) const;

        int rowCount(void) const;
        int columnCount(void) const;

        QCSVRow     header(void) const;
        QStringList headersList(void) const;
        QCSVRow&    row(int index);
        QCSVColumn& column(int index);
        void        addRow(QCSVRow const& row);
        void        eraseRow(int rowindex);
        void        reset(void);

        // Getters
        QString fileName(void) const;
        QChar  separator(void) const;
        QString::SplitBehavior behavior(void) const;

        QString columnName(int column) const;
        int columnIndice(const QString& columnName) const;

        // Operators overload
        QCSVColumn& operator[](QString const& columnName);
        QCSVColumn& operator[](int column);

    protected:

        void writeRow(QTextStream& out, QCSVRow const& row) const;

        QString _fileName;
        QChar   _separator;
        QString::SplitBehavior _behavior;

        QCSVDataBase _content;
        QCSVColumn   _column;
};

#endif /* __QCSVPARSER_HPP__ */
