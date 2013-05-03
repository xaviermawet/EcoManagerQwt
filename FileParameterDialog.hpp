#ifndef __FILEPARAMETERDIALOG_HPP__
#define __FILEPARAMETERDIALOG_HPP__

#if QT_VERSION >= 0x050000 //  0xMMNNPP (MM = major, NN = minor, PP = patch)
    #include <QtWidgets>
#else
    #include <QtGui>
#endif

#include "Utils/QException.hpp"

#define GPS_KEY             "GPS"
#define SPEED_KEY           "SPEED"
#define MEGASQUIRT_DAT_KEY  "MEGASQUIRT_DAT"
#define MEGASQUIRT_CSV_KEY  "MEGASQUIRT_CSV"

#define DEFAULT_GPS_FILENAME            "gps.csv"
#define DEFAULT_SPEED_FILENAME          "speed.csv"
#define DEFAULT_MEGASQUIRT_DAT_FILENAME "megasquirt.dat"
#define DEFAULT_MEGASQUIRT_CSV_FILENAME "megasquirt.csv"

namespace Ui {
class FileParameterDialog;
}

class FileParameterDialog : public QDialog
{
    Q_OBJECT
    
    public:

        explicit FileParameterDialog(QWidget* parent = NULL);
        virtual ~FileParameterDialog(void);

    protected:

        void readSettings(void);
        void writeSettings(void) const;

    private slots:

        void on_savePushButton_clicked(void);

    protected:

        Ui::FileParameterDialog *ui;
};

#endif /* __FILEPARAMETERDIALOG_HPP__ */
