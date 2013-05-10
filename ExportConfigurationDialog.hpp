#ifndef __EXPORTCONFIGURATIONDIALOG_HPP__
#define __EXPORTCONFIGURATIONDIALOG_HPP__

#include <QDialog>

namespace Ui {
class ExportConfigurationDialog;
}

class ExportConfigurationDialog : public QDialog
{
    Q_OBJECT
    
    public:

        explicit ExportConfigurationDialog(QWidget* parent = NULL);
        virtual ~ExportConfigurationDialog(void);

        int precision(void) const;

    protected:

        Ui::ExportConfigurationDialog *ui;
};

#endif // EXPORTCONFIGURATIONDIALOG_HPP
