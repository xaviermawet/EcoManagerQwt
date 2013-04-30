/* TODO :
 * ------------------------
 * Quand on delete une competition, il n'y a pas de "reloadRaceView"
 * donc on voit tjs la liste des courses ... pas bon ça
 */
#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include "Map/MapScene.hpp"
#include "Map/MapFrame.hpp"
#include "Plot/PlotFrame.hpp"
#include "Plot/HorizontalScale.hpp"
#include "Plot/VerticalScale.hpp"
#include "DBModule/ImportModule.hpp" // ----- a supprimer apres
#include "DBModule/DataBaseImportModule.hpp"
#include "CompetitionEntryDialog.hpp"
#include "CompetitionProxyModel.hpp"
#include "Common/GroupingTreeModel.hpp"
#include "Common/TreeLapInformationModel.hpp"
#include "DBModule/ExportModule.hpp"
#include "Common/ColorizerProxyModel.hpp"
#include "Map/SampleLapViewer.hpp"
#include "LapInformationProxyModel.hpp"
#include "Common/LapInformationTreeModel.hpp"
#include "LapDataCompartor.hpp"
#include "Utils/DataBaseManager.hpp"
#include "Utils/QCSVParser.hpp"

#include <QtGui>
#include <QtSql>
#include <float.h>

namespace Ui {
class MainWindow;
}

typedef QMap<QString, QVariant> TrackIdentifier;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow(void);

        virtual bool eventFilter(QObject* src, QEvent* event);

    private slots:

        // Autoconnect
        void on_actionAboutQt_triggered(void);
        void on_actionQuit_triggered(void);
        void on_actionImport_triggered(void);
        void on_actionImport_triggered_old(void);
        void on_actionAboutEcoManager2013_triggered(void);
        void on_actionExportConfigurationModule_triggered(void);
        void on_actionExportData_triggered(void);
        void on_raceView_doubleClicked(const QModelIndex& index);
        void on_actionDelimitingSectors_triggered(void);
        void on_raceView_customContextMenuRequested(const QPoint &pos);
        void on_actionDisplayRaceTableData_triggered(bool checked);
        void on_actionDisplayRaceTableUnder_triggered();
        void on_actionDisplayRaceTableAbove_triggered();
        void on_actionDisplayRaceTableOnRight_triggered();
        void on_actionDisplayRaceTableOnLeft_triggered();
        void on_actionDisplayRaceView_triggered(bool checked);
        void on_actionSaveCurrentLayout_triggered(void);
        void on_actionConfiguredLayout1_triggered(void);
        void on_actionConfiguredLayout2_triggered(void);
        void on_actionConfiguredLayout3_triggered(void);
        void on_actionConfiguredLayout4_triggered(void);
        void on_actionLapDataEraseTable_triggered(void);
        void on_actionLapDataTableResizeToContents_triggered(bool checked);
        void on_actionClearAllData_triggered(void);
        void on_raceTable_customContextMenuRequested(const QPoint &pos);
        void on_actionLapDataComparaison_triggered(void);
        void on_raceTable_doubleClicked(const QModelIndex &index);
        void on_menuLapDataTable_aboutToShow(void);
        void on_actionLapDataSelectAll_triggered(bool checked);
        void on_actionLapDataDrawSectors_triggered(void);
        void on_actionLapDataDisplayInAllViews_triggered(void);
        void on_actionLapDataExportToCSV_triggered(void);
        void on_menuEditRaceView_aboutToShow(void);
        void on_actionRaceViewDisplayLap_triggered(void);
        void on_actionRaceViewRemoveLap_triggered(void);
        void on_actionRaceViewExportLapDataInCSV_triggered(void);
        void on_actionRaceViewDeleteRace_triggered(void);
        void on_actionRaceViewDeleteRacesAtSpecificDate_triggered(void);
        void on_actionDeleteCurrentCompetition_triggered(void);
        void on_actionNewProject_triggered(void);
        void on_actionOpenProject_triggered(void);

        // for debug purpose --- should be deleted at the end
        void on_actionCompter_le_nombre_de_tours_triggered(void);
        void on_actionCompter_le_nombre_de_courses_triggered(void);
        void on_actionPRAGMA_foreign_keys_triggered(void);
        void on_actionListing_des_courses_triggered(void);
        void on_actionListing_des_competitions_triggered(void);
        void on_actionCompter_tous_les_tuples_de_toutes_les_tables_triggered(void);

        // Personal slots
        void loadCompetition(int index);
        void removeSector(const QString& competitionName, int sectorNum);
        void addSector(QString, int, IndexedPosition, IndexedPosition);
        void updateSector(QString, int, IndexedPosition, IndexedPosition);

        void displayLapInformation(float timeValue, const QVariant& trackId);
        void displayLapInformation(float lowerTimeValue, float upperTimeValue,
                                   const QVariant &trackId);

        void deleteRace(int raceId);
        void deleteRaces(QVariantList listRaceId);

        void on_actionExecuter_une_requete_triggered(void);

private:

        void centerOnScreen(void);
        void createRaceView(void);
        void createToolsBar(void);
        void createMapZone(void);
        void createPlotZone(void);
        void createMegaSquirtZone(void);
        void createRaceTable(void);
        void readSettings(const QString& settingsGroup);
        void writeSettings(const QString& settingsGroup) const;
        void displayDataLap(void);
        void connectSignals(void);
        void reloadRaceView(void);
        void loadSectors(const QString& competitionName);
        void highlightPointInAllView(const QModelIndex& index);
        void removeTrackFromAllView(QMap<QString, QVariant> const& trackId);

        void updateDataBase(QString const& dbFilePath,
                           bool(*dataBaseAction)(QString const&));

        double getCurrentCompetitionWheelPerimeter(void) const;

        // TODO : Jeter une exception avec le message d'erreur au lieu d'un bool
        bool getAllDataFromSpeed(
                const TrackIdentifier& trackId, float lowerTimeValue,
                float upperTimeValue, QList< QList<QVariant> >& data);

        void exportLapDataToCSV(const TrackIdentifier& trackId,
                                float lowerTimeValue, float upperTimeValue);

    protected:

        virtual void closeEvent(QCloseEvent* event);

        Ui::MainWindow* ui;

        QComboBox* competitionBox;
        QString currentCompetition;
        QList<TrackIdentifier> currentTracksDisplayed;

        // Mapping
        MapFrame* mapFrame;

        // Plot
        PlotFrame* distancePlotFrame;
        PlotFrame* timePlotFrame;
        PlotFrame* megaSquirtPlotFrame;

        // Models
        QSqlTableModel* sectorModel;
        QSqlTableModel* competitionNameModel;

        // Personal Models
        GroupingTreeModel* competitionModel;
        LapInformationTreeModel* raceInformationTableModel; //TreeLapInformationModel* raceInformationTableModel;

        // RaceView item identifier
        QVariant raceViewItemidentifier;
};

#endif /* __MAINWINDOW_HPP__ */
