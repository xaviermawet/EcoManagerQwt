#include "ExportConfigurationDialog.hpp"
#include "ui_ExportConfigurationDialog.h"

ExportConfigurationDialog::ExportConfigurationDialog(QWidget* parent) :
    QDialog(parent), ui(new Ui::ExportConfigurationDialog)
{
    // GUI Configuration
    this->ui->setupUi(this);
}

ExportConfigurationDialog::~ExportConfigurationDialog(void)
{
    delete this->ui;
}

int ExportConfigurationDialog::precision(void) const
{
    return this->ui->precisionSlider->value();
}
