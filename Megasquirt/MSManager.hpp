#ifndef __MSMANAGER_HPP__
#define __MSMANAGER_HPP__

#if QT_VERSION >= 0x050000 //  0xMMNNPP (MM = major, NN = minor, PP = patch)
    #include <QtWidgets>
#else
    #include <QtGui>
#endif

#include "MSDataConverter.hpp"
#include "../Utils/QException.hpp"
#include "../Utils/QCSVParser.hpp"

#define DATA_SIZE	169

class MSManager
{
    public:

        MSManager(void);

        // Getters
        int count(void) const;
        int fieldIndice(QString const& field) const;
        QStringList fields(void) const;

        const MSDataConverter& dataConverter(int fieldIndice) const;
        const MSDataConverter& dataConverter(QString const& field) const;

        const MSDataConverter& operator[](QString const& field) const;
        const MSDataConverter& operator[](int fieldIndice) const;

        // File operations
        void datToCSV(QString const& datFile, QString const& CSVFile,
                      QStringList const& fields = QStringList()) const;

    protected:

        QList<MSDataConverter> dataConverters;
};

#endif /* __MSMANAGER_HPP__ */
