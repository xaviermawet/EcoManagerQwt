#include "FileParameterDialog.hpp"
#include "ui_FileParameterDialog.h"

FileParameterDialog::FileParameterDialog(QWidget* parent) :
    QDialog(parent), ui(new Ui::FileParameterDialog)
{
    // Gui Configuration
    this->ui->setupUi(this);

    // Load file information from Settings
    this->readSettings();
}

FileParameterDialog::~FileParameterDialog(void)
{
    delete this->ui;
}

void FileParameterDialog::readSettings(void)
{
    QSettings settings;

    settings.beginGroup("Files");

    if(!settings.contains(GPS_KEY))
        settings.setValue(GPS_KEY, DEFAULT_GPS_FILENAME);
    this->ui->GPSFileLineEdit->setText(settings.value(GPS_KEY).toString());

    if(!settings.contains(SPEED_KEY))
        settings.setValue(SPEED_KEY, DEFAULT_SPEED_FILENAME);
    this->ui->speedFileLineEdit->setText(settings.value(SPEED_KEY).toString());

    if(!settings.contains(MEGASQUIRT_DAT_KEY))
        settings.setValue(MEGASQUIRT_DAT_KEY, DEFAULT_MEGASQUIRT_DAT_FILENAME);
    this->ui->megasquirtFileLineEdit->setText(
                settings.value(MEGASQUIRT_DAT_KEY).toString());

    if(!settings.contains(MEGASQUIRT_CSV_KEY))
        settings.setValue(MEGASQUIRT_CSV_KEY, DEFAULT_MEGASQUIRT_CSV_FILENAME);
    this->ui->generateMegasquirtFileLineEdit->setText(
                settings.value(MEGASQUIRT_CSV_KEY).toString());

    settings.endGroup();
}

void FileParameterDialog::writeSettings(void) const
{
    // Check if all file have a name
    try
    {
        if(this->ui->GPSFileLineEdit->text().isEmpty())
            throw QException(this->ui->GPSFileLabel->text());

        if(this->ui->speedFileLineEdit->text().isEmpty())
            throw QException(this->ui->speedFileLabel->text());

        if(this->ui->megasquirtFileLineEdit->text().isEmpty())
            throw QException(this->ui->megasquirtFileLabel->text());

        if(this->ui->generateMegasquirtFileLineEdit->text().isEmpty())
            throw QException(this->ui->generateMegasquirtFileLabel->text());
    }
    catch(QException const& ex)
    {
        throw QException(tr("Vous devez entrer un nom pour le fichier :\n")
                         + ex.what());
    }

    // Write settings
    QSettings settings;

    settings.beginGroup("Files");

    settings.setValue(GPS_KEY,
                      this->ui->GPSFileLineEdit->text());
    settings.setValue(SPEED_KEY,
                      this->ui->speedFileLineEdit->text());
    settings.setValue(MEGASQUIRT_DAT_KEY,
                      this->ui->megasquirtFileLineEdit->text());
    settings.setValue(MEGASQUIRT_CSV_KEY,
                      this->ui->generateMegasquirtFileLineEdit->text());

    settings.endGroup();
}

void FileParameterDialog::on_savePushButton_clicked(void)
{
    try
    {
        // Save the settings (file names)
        this->writeSettings();

        // Close the dialog
        this->accept();
    }
    catch(QException const& ex)
    {
        QMessageBox::warning(this, tr("Sauvegarde impossible"), ex.what());
    }
}
