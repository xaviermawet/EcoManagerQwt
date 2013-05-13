#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    competitionBox(NULL), mapFrame(NULL), distancePlotFrame(NULL),
    timePlotFrame(NULL), megasquirtDataPlot(NULL), sectorModel(NULL),
    competitionNameModel(NULL), competitionModel(NULL),
    raceInformationTableModel(NULL)
{
    QCoreApplication::setOrganizationName("EcoMotion");
    QCoreApplication::setApplicationName("EcoManager2013");

    // GUI Configuration
    this->ui->setupUi(this);
    connect(this->ui->plotsTabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(updateMenus()));

    // Reharge le dernier "projet" s'il existe tjrs
    if (!DataBaseManager::restorePreviousDataBase())
    {
        this->ui->actionImport->setVisible(false);
        this->ui->actionExportSectors->setVisible(false);
    }

    // Display Configuration
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // Building of the parts of the MainWindow
    this->createRaceView();
    this->createRaceTable();
    this->createMapZone();
    this->createPlotsZone();
    this->createToolsBar();
    this->createPlotLegendContextMenu();

    // Connect all the signals
    this->connectSignals();

    this->readSettings();
    this->updateMenus();
    this->centerOnScreen();
}

MainWindow::~MainWindow(void)
{
    delete this->ui;
}

/* Interception des events clavier emis sur la sectorView afin de pouvoir repercuter
 * la suppression logique d'un secteur sur la BD */
bool MainWindow::eventFilter(QObject* src, QEvent* event)
{
    if (src == this->ui->sectorView && event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Delete)
        {
            // Check if at lease one sector is selected
            QModelIndexList selectedRows = this->ui->sectorView->selectionModel()->selectedRows();
            if (selectedRows.isEmpty())
            {
                QMessageBox::information(this,
                                       tr("Impossible de supprimer un secteur"),
                 tr("Vous devez sélectionner au moins un secteur à supprimer"));

                return QMainWindow::eventFilter(src, event);
            }

            QModelIndex selectedSector = selectedRows.first();

            if (selectedSector.isValid())
            {
                QSqlRecord sectorRec = sectorModel->record(selectedSector.row());
                int numSect = sectorRec.value(sectorModel->fieldIndex("num")).toInt();
                this->mapFrame->scene()->mergeSector(numSect - 1, numSect);
            }
        }
    }

    return QMainWindow::eventFilter(src, event);
}

void MainWindow::on_actionAboutQt_triggered(void)
{
    qApp->aboutQt();
}

void MainWindow::on_actionQuit_triggered(void)
{
    // Save the state of the mainWindow and its widgets
    this->writeSettings();

    qApp->quit();
}

// importData
void MainWindow::on_actionImport_triggered(void)
{
    QString raceDataDirPath = QFileDialog::getExistingDirectory(this,
              tr("Sélectionnez le dossier contenant les données de la course"));

    if (raceDataDirPath.isEmpty()) // User canceled
        return;

    try
    {
        // Manage data importation
        DataBaseImportModule raceDataImporter;
        raceDataImporter.checkFolderContent(raceDataDirPath);

        // Set import settings
        CompetitionEntryDialog dial;
        if(dial.exec() != QDialog::Accepted) // User canceled
            return;

        if(dial.isNewlyCreated())
        {
            // Create new entry for the competition in the database
            raceDataImporter.createCompetition(dial.competitionName(),
                                               dial.wheelRadius() / 100.0,
                                               dial.place());

            // Update combobox that contains the list of competition names
            this->competitionNameModel->select();

            // Display the newest competition
            this->competitionBox->setCurrentIndex(
                        this->competitionBox->count() - 1);
        }

        Race newRace(dial.competitionName());
        newRace.setDate(dial.date());

        // Add race data to the data base
        raceDataImporter.addRace(newRace, raceDataDirPath);

        // Update combobox that contains the list of competition names
       this->reloadRaceView();
    }
    catch(QException const& ex)
    {
        QMessageBox::warning(this, tr("Importation annulée"), ex.what());
    }
}

void MainWindow::on_actionImport_triggered_old(void)
{
    // Select the directory that content race data and enter race information
    CompetitionEntryDialog dial;
    QString raceDirectoryPath = QFileDialog::getExistingDirectory(this);
    if (raceDirectoryPath.isEmpty() || dial.exec() != QDialog::Accepted)
    {
        QMessageBox::information(this, tr("Importation annulée"),
                                 tr("L'importation des données de la course"
                                    " a été <strong>annulée</strong>"));
        return;
    }

    ImportModule raceInformationImporter;

    if (dial.isNewlyCreated())
    {
        // Create new entry for the competition in the database
        raceInformationImporter.createCompetition(dial.competitionName(),
                                                  dial.wheelRadius()/ 100.0,
                                                  dial.place());

        // Update combobox taht contains the list of competition names
        this->competitionNameModel->select();
    }

    Race newRace(dial.competitionName());
    newRace.setDate(dial.date());

    raceInformationImporter.addRace(newRace, raceDirectoryPath);
    if (!raceInformationImporter.importSuceed())
    {
        QMessageBox::warning(this, tr("Erreur d'importation"),
                             raceInformationImporter.getErrorString());
    }
    else
    {
        this->reloadRaceView();
    }

}

void MainWindow::on_actionAboutEcoManager2013_triggered(void)
{
    QMessageBox::information(this, "Action About EcoManager 2013",
                             "A propos du project EcoManager 2013 ....");
}

void MainWindow::on_actionExportSectors_triggered(void)
{
    try
    {
        // Get all competition names from database
        QSqlQuery query("SELECT name FROM COMPETITION");
        if(!query.exec())
            throw QException(tr("Impossible de récupérer la liste des compétitions"));

        QStringList competitionNames;
        while(query.next())
            competitionNames << query.value(0).toString();

        if (competitionNames.count() == 0)
            throw QException(tr("Le projet ne contient aucune compétition"));

        // Get destination directory
        QString destDirPath = QFileDialog::getExistingDirectory(this,
           tr("Sélectionnez le dossier dans lequel exporter les données secteurs"),
           QDir::homePath());

        if(destDirPath.isEmpty()) // User canceled
            return;

        // Select a competition
        bool ok(false);
        QString competitionName = QInputDialog::getItem(
                    this, tr("Nom de la compétition"),
                    tr("Choisissez une compétition : "), competitionNames,
                    0, false, &ok);

        if (ok && !competitionName.isEmpty())
            ExportModule::buildSectorOutput(competitionName, destDirPath);
    }
    catch(QException const& ex)
    {
        QMessageBox::warning(this, tr("Exportation impossible"), ex.what());
    }
}

void MainWindow::on_raceView_pressed(const QModelIndex& index)
{
    /* The signal is only emitted when the index is valid */

    // Date clicked
    if (!index.parent().isValid())
    {
        /* ------------------------------------------------------------------ *
         *                         Get date identifier                        *
         * ------------------------------------------------------------------ */

        QDate date = this->competitionModel->data(
                    this->competitionModel->index(index.row(), 0,
                                                  index.parent())).toDate();

        this->raceViewItemidentifier = QVariant::fromValue(date);

        qDebug() << "Date = " << date.toString(Qt::SystemLocaleShortDate);
    }
    else if (!index.parent().parent().isValid())
    {
        /* ------------------------------------------------------------------ *
         *                         Get race identifier                        *
         * ------------------------------------------------------------------ */

        // Get race id
        int raceId = this->competitionModel->data(
                        this->competitionModel->index(0, 1, index)).toInt();

        this->raceViewItemidentifier = QVariant::fromValue(raceId);

        qDebug() << "Course : Race id = " << raceId
                 << " race num = " << this->competitionModel->data(index).toInt();
    }
    else
    {
        /* ------------------------------------------------------------------ *
         *                        Get track identifier                        *
         * ------------------------------------------------------------------ */

        int ref_race = competitionModel->data(
                    competitionModel->index(index.row(), 1,
                                            index.parent())).toInt();
        int ref_lap = competitionModel->data(
                    competitionModel->index(index.row(), 2,
                                            index.parent())).toInt();

        int race_num = competitionModel->data(
                    competitionModel->index(index.parent().row(), 0,
                                            index.parent().parent())).toInt();

        TrackIdentifier trackIdentifier;
        trackIdentifier["race"] = ref_race;
        trackIdentifier["lap"] = ref_lap;
        trackIdentifier["race_num"] = race_num;

        this->raceViewItemidentifier = QVariant::fromValue(trackIdentifier);

        // Update the megasquirt labels for the current trackidentifier
        this->ui->raceNumberLabel->setText(QString::number(race_num));
        this->ui->lapNumberLabel->setText(QString::number(ref_lap));

        qDebug() << "Tour : race num = "
                 << race_num << " race id = "
                 << ref_race << " lap = " << ref_lap;
    }
}

/* Event that occured when the user double click on a race tree view item
 * Only lap can be double-clicked */
void MainWindow::on_raceView_doubleClicked(const QModelIndex& index)
{
    Q_UNUSED(index);

    // Check if a valid row has been double clicked
    QModelIndexList rowsSelected = this->ui->raceView->selectionModel()->selectedRows();
    if (rowsSelected.count() <= 0)
        QMessageBox::information(this, tr("Erreur"),
                                 tr("Vous devez double-cliquer sur un tour"));
    else
        this->displayDataLap();
}

// chooseSampleLap
void MainWindow::on_actionDelimitingSectors_triggered(void)
{
    SampleLapViewer* dial = new SampleLapViewer(currentCompetition, this);
    dial->setModal(true);
    dial->resize(650, 400);

    // if the user choose a new sample lap
    if (dial->exec())
    {
        // Get track identifier
        QPair<int, int> refLap = dial->selectedReferencesLap();
        QSqlQuery posBoundaryQuery;
        posBoundaryQuery.prepare("select min(id), max(id) from POSITION where ref_lap_race = ? and ref_lap_num = ?");
        posBoundaryQuery.addBindValue(refLap.first);
        posBoundaryQuery.addBindValue(refLap.second);

        if (!posBoundaryQuery.exec() || ! posBoundaryQuery.next())
        {
            qWarning() << posBoundaryQuery.lastQuery() << posBoundaryQuery.lastError();
            return;
        }

        // Check if sectors alreay exists for the competition
        QSqlQuery existingSectorQuery;
        existingSectorQuery.prepare("select count(*) from SECTOR where ref_compet = ?");
        existingSectorQuery.addBindValue(currentCompetition);

        if (existingSectorQuery.exec() && existingSectorQuery.next())
        {
            int nbExistingSectors = existingSectorQuery.value(0).toInt();

            if (nbExistingSectors > 0)
            {
                // delete all existing sectors for the competition
                QSqlQuery delExistingSectorsQuery;
                delExistingSectorsQuery.prepare("delete from SECTOR where ref_compet = ?");
                delExistingSectorsQuery.addBindValue(currentCompetition);

                if (!delExistingSectorsQuery.exec())
                {
                    qWarning() << delExistingSectorsQuery.lastQuery() << delExistingSectorsQuery.lastError();
                    return;
                }
            }

            // Create new sector for the competition
            QSqlQuery sectorCreationQuery;
            sectorCreationQuery.prepare("insert into SECTOR (num, ref_compet, start_pos, end_pos) values (?, ?, ?, ?)");
            sectorCreationQuery.addBindValue(0);
            sectorCreationQuery.addBindValue(currentCompetition);
            sectorCreationQuery.addBindValue(posBoundaryQuery.value(0).toInt());
            sectorCreationQuery.addBindValue(posBoundaryQuery.value(1).toInt());

            if (!sectorCreationQuery.exec())
            {
                qWarning() << sectorCreationQuery.lastQuery() << sectorCreationQuery.lastError();
            }
            else
            {
                // Erase previous sectors from the map scene
                if (this->mapFrame->scene()->hasSectors())
                    this->mapFrame->scene()->clearSectors();

                // Display new sector
                loadSectors(currentCompetition);
            }
        }
        else
        {
            qWarning() << existingSectorQuery.lastQuery() << existingSectorQuery.lastError();
        }
    }
}

void MainWindow::on_raceView_customContextMenuRequested(const QPoint &pos)
{
    this->ui->menuEditRaceView->exec(
                this->ui->raceView->viewport()->mapToGlobal(pos));
}

void MainWindow::on_actionDisplayRaceTableData_triggered(bool checked)
{
    this->ui->raceTable->setVisible(checked);
}

void MainWindow::on_actionDisplayRaceTableUnder_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Vertical);
    this->ui->MapPlotAndRaceSplitter->insertWidget(1, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceTableAbove_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Vertical);
    this->ui->MapPlotAndRaceSplitter->insertWidget(0, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceTableOnRight_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Horizontal);
    this->ui->MapPlotAndRaceSplitter->insertWidget(1, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceTableOnLeft_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Horizontal);
    this->ui->MapPlotAndRaceSplitter->insertWidget(0, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceView_triggered(bool checked)
{
    this->ui->raceFrame->setVisible(checked);
}

void MainWindow::on_actionSaveCurrentLayout_triggered(void)
{
    bool ok(false);

    QStringList listSavedLayouts;
    listSavedLayouts << this->ui->actionConfiguredLayout1->text()
                     << this->ui->actionConfiguredLayout2->text()
                     << this->ui->actionConfiguredLayout3->text()
                     << this->ui->actionConfiguredLayout4->text();

    QString layoutSelected = QInputDialog::getItem(
                this, tr("Sauvegarde de la disposition courante"),
                tr("Choisissez l'emplacement dans lequel sauver la disposition "
                   "courante"), listSavedLayouts, 0, false, &ok);
    if (!ok)
        return;

    // Sauvegarde des paramètres d'affichage
    this->writeLayoutSettings(layoutSelected);
    QMessageBox::information(this, tr("Sauvegarde de la disposition courante"),
                             tr("La disposition courante a correctement été "
                                "sauvée dans ") + layoutSelected);
}

void MainWindow::on_actionConfiguredLayout1_triggered(void)
{
    this->readLayoutSettings(this->ui->actionConfiguredLayout1->text());
}

void MainWindow::on_actionConfiguredLayout2_triggered(void)
{
    this->readLayoutSettings(this->ui->actionConfiguredLayout2->text());
}

void MainWindow::on_actionConfiguredLayout3_triggered(void)
{
    this->readLayoutSettings(this->ui->actionConfiguredLayout3->text());
}

void MainWindow::on_actionConfiguredLayout4_triggered(void)
{
    this->readLayoutSettings(this->ui->actionConfiguredLayout4->text());
}

void MainWindow::on_actionLapDataEraseTable_triggered(void)
{
//    // Erase all highlited point or sector on the mapping view
//    this->mapFrame->scene()->clearSceneSelection();

//    // Erase all highlited point on the mapping view
//    this->distancePlotFrame->clearSecondaryCurves();
//    this->timePlotFrame->clearSecondaryCurves();

    // Remove laps information from the table
    this->raceInformationTableModel->removeRows(
                0, this->raceInformationTableModel->rowCount());
}

void MainWindow::on_actionLapDataTableResizeToContents_triggered(bool checked)
{
    if (checked)
        this->ui->raceTable->header()->setResizeMode(QHeaderView::ResizeToContents);
    else
        this->ui->raceTable->header()->setResizeMode(QHeaderView::Interactive);
}

void MainWindow::on_actionClearAllData_triggered(void)
{
    qDebug() << "On efface tout";

    // Clear the tracks of the mapping view
    this->mapFrame->scene()->clearTracks();

    // Clear the list of all tracks currently displayed
    this->currentTracksDisplayed.clear();

    // Remove laps information from the table
    this->on_actionLapDataEraseTable_triggered();

    this->mapFrame->scene()->clearSectors(); // clear sectors

    // clear the sector view
    if (this->sectorModel)
    {
        this->ui->sectorView->setVisible(false);
        this->sectorModel->clear();
        this->ui->sectorView->update();
    }

    // clear the curves of the graphic views
    foreach (AdvancedPlot* plot, this->plots)
        plot->clearcurves();
}

void MainWindow::on_raceTable_customContextMenuRequested(const QPoint &pos)
{
    this->ui->menuLapDataTable->exec(
                this->ui->raceTable->viewport()->mapToGlobal(pos));
}

void MainWindow::on_actionLapDataComparaison_triggered(void)
{
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows(0);

    if (rowsSelectedIndexes.count() != 2)
        return;

    int lapNum, raceNum;
    QModelIndex LapNumModelIndex;
    QModelIndex RaceNumModelIndex;

    // Get lap and race num for the first selected item
    LapNumModelIndex  = rowsSelectedIndexes.at(0).parent();
    RaceNumModelIndex = LapNumModelIndex.parent();

    lapNum = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(LapNumModelIndex.row(),
                                                 0, RaceNumModelIndex)).toInt();
    raceNum = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(RaceNumModelIndex.row(),
                                                       0)).toInt();

    QVector<QVariant> row1 = this->raceInformationTableModel->rowData(
                rowsSelectedIndexes.at(0));
    QVector<QVariant> row2 = this->raceInformationTableModel->rowData(
                rowsSelectedIndexes.at(1));

    LapDataCompartor ldc(raceNum, lapNum, this);
    ldc.addLapsData(row1.toList(), row2.toList());
    ldc.exec();
}

void MainWindow::on_raceTable_doubleClicked(const QModelIndex &index)
{
    // Check if a valid row has been double clicked
    QModelIndexList rowsSelected = this->ui->raceTable->selectionModel()->selectedRows();
    if (rowsSelected.count() <= 0)
        return;

    this->highlightPointInAllView(index);
}

void MainWindow::on_menuLapDataTable_aboutToShow(void)
{
    // Récupérer les index de tous les éléments sélectionnés
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows();

    bool multipleRowsSelected = rowsSelectedIndexes.count() > 0;
    bool comparaisonVisible   = rowsSelectedIndexes.count() == 2 &&
                                rowsSelectedIndexes.at(0).parent() ==
                                rowsSelectedIndexes.at(1).parent();

    // Afficher dans toutes les vues
    this->ui->actionLapDataDisplayInAllViews->setVisible(multipleRowsSelected);

    // Retirer les données sélectionnées (du tableau et des vues)
    this->ui->actionLapDataExportToCSV->setVisible(comparaisonVisible);

    /* On ne porpose de faire une comparaison entre deux données du tableau
     * si et seulement si ce sont des données issues d'une meme course et
     * du meme tour */
    this->ui->actionLapDataComparaison->setVisible(comparaisonVisible);
    this->ui->actionLapDataDrawSectors->setVisible(comparaisonVisible);
}

void MainWindow::on_actionLapDataSelectAll_triggered(bool checked)
{
    checked ? this->ui->raceTable->selectAll() :
              this->ui->raceTable->clearSelection();
}

void MainWindow::on_actionLapDataDrawSectors_triggered(void)
{
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows(0);

    if (rowsSelectedIndexes.count() != 2)
        return;

    QModelIndex LapNumModelIndex;
    QModelIndex RaceNumModelIndex;

    // Get lap and race num for the first selected item
    LapNumModelIndex  = rowsSelectedIndexes.at(0).parent();
    RaceNumModelIndex = LapNumModelIndex.parent();

    TrackIdentifier trackId;
    trackId["race"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(RaceNumModelIndex.row(),
                                                       0)).toInt();

    trackId["lap"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(LapNumModelIndex.row(),
                                                 0, RaceNumModelIndex)).toInt();

    // ====================================================================================
    /* NOTE : Le code qui suit ne devrait pas etre là. Je devrais modifier le
     * model du tableau afin que chaque élément contienne le race number
     * (ou uniquement la ligne "course" du tableau. je n'ai donc pas encore
     * corrigé les problèmes du au fait que j'ai ajouté un champ dans le trackid
     */
    QSqlQuery query("SELECT num FROM race WHERE id = ?");
    query.addBindValue(trackId["race"]);

    if(!query.exec() || !query.next())
    {
        QMessageBox::warning(this, tr("Une erreur est survenue"),
                             tr("Impossible de récupérer le numéro de la course"));
        return;
    }

    trackId["race_num"] = query.value(0).toInt();
    qDebug() << "Numéro du tour = " << query.value(0).toInt();
    // ====================================================================================

    // Get the time in milliseconds for the selected item
    QModelIndex rowIndex = rowsSelectedIndexes.at(0);
    float time1 = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(
                    rowIndex.row(), 2, rowIndex.parent())).toFloat();

    rowIndex = rowsSelectedIndexes.at(1);
    float time2 = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(
                    rowIndex.row(), 2, rowIndex.parent())).toFloat();


    //this->mapFrame->scene()->clearSceneSelection();
    this->mapFrame->scene()->highlightSector(time1, time2, trackId);

    //this->distancePlotFrame->scene()->clearPlotSelection();
//    this->distancePlotFrame->scene()->highlightSector(time1, time2, trackId); // FIXME : méthode qui utilise un arondissement car prévu pour des données de temps issues de la vue mapping

    //this->timePlotFrame->scene()->clearPlotSelection();
//    this->timePlotFrame->scene()->highlightSector(time1, time2, trackId); // FIXME : méthode qui utilise un arondissement car prévu pour des données de temps issues de la vue mapping
}

void MainWindow::on_actionLapDataDisplayInAllViews_triggered(void)
{
    // Récupérer les index de tous les éléments sélectionnés
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows();

    // Mise en évidence de tous les points sélectionnés dans toutes les vues
    foreach (QModelIndex index, rowsSelectedIndexes)
        this->highlightPointInAllView(index);
}

void MainWindow::on_actionLapDataExportToCSV_triggered(void)
{
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows();

    if (rowsSelectedIndexes.count() != 2)
        return;

    int lapNum, raceNum;
    QModelIndex LapNumModelIndex;
    QModelIndex RaceNumModelIndex;

    // Get lap and race num for the first selected item
    LapNumModelIndex  = rowsSelectedIndexes.at(0).parent();
    RaceNumModelIndex = LapNumModelIndex.parent();

    lapNum = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(LapNumModelIndex.row(),
                                                 0, RaceNumModelIndex)).toInt();
    raceNum = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(RaceNumModelIndex.row(),
                                                       0)).toInt();

    // Create trackidentifier
    TrackIdentifier trackId;
    trackId["race"] = raceNum;
    trackId["lap"]  = lapNum;

    float time1 = this->raceInformationTableModel->rowData(
                rowsSelectedIndexes.at(0)).at(2).toFloat();

    float time2 = this->raceInformationTableModel->rowData(
                rowsSelectedIndexes.at(1)).at(2).toFloat();

    this->exportLapDataToCSV(trackId, qMin(time1, time2), qMax(time1, time2));
}

void MainWindow::on_menuEditRaceView_aboutToShow(void)
{
    // Par défaut, toutes les actions du menu sont masquées
    this->ui->actionRaceViewDisplayLap->setVisible(false);
    this->ui->actionRaceViewRemoveLap->setVisible(false);
    this->ui->actionRaceViewExportLapDataInCSV->setVisible(false);
    this->ui->actionRaceViewDeleteRace->setVisible(false);
    this->ui->actionRaceViewDeleteRacesAtSpecificDate->setVisible(false);

    // Si la tree view ne contient aucune model
    if(this->ui->raceView->selectionModel() == NULL)
        return;

    if (this->raceViewItemidentifier.canConvert<QDate>())
    {
        /* ------------------------------------------------------------------ *
         *                         Get date identifier                        *
         * ------------------------------------------------------------------ */

        QDate date = this->raceViewItemidentifier.value<QDate>();

        this->ui->actionRaceViewDeleteRacesAtSpecificDate->setText(
                    tr("Supprimer les courses effectuées à ") +
                    this->currentCompetition + " le " +
                    date.toString(Qt::SystemLocaleShortDate));

        this->ui->actionRaceViewDeleteRacesAtSpecificDate->setVisible(true);
    }
    else if (this->raceViewItemidentifier.canConvert<int>())
    {
        /* ------------------------------------------------------------------ *
         *                         Get race identifier                        *
         * ------------------------------------------------------------------ */

        // Récupération de l'élément sélectionné
        QModelIndex curIndex = this->ui->raceView->selectionModel()->currentIndex();
        if (!curIndex.isValid())
            return;

        // Get race number
        int raceNum = this->competitionModel->data(curIndex).toInt();

        this->ui->actionRaceViewDeleteRace->setText(
                    tr("Supprimer la course ") + QString::number(raceNum));
        this->ui->actionRaceViewDeleteRace->setVisible(true);
    }
    else if(this->raceViewItemidentifier.canConvert< TrackIdentifier >())
    {
        /* ------------------------------------------------------------------ *
         *                        Get track identifier                        *
         * ------------------------------------------------------------------ */

        TrackIdentifier trackIdentifier =
                this->raceViewItemidentifier.value< TrackIdentifier >();

        bool lapAlreadyDisplayed = this->currentTracksDisplayed.contains(
                    trackIdentifier);

        this->ui->actionRaceViewDisplayLap->setVisible(!lapAlreadyDisplayed);
        this->ui->actionRaceViewRemoveLap->setVisible(lapAlreadyDisplayed);
        this->ui->actionRaceViewExportLapDataInCSV->setVisible(true);
    }
}

void MainWindow::on_actionRaceViewDisplayLap_triggered(void)
{
    this->displayDataLap();
}

void MainWindow::on_actionRaceViewExportLapDataInCSV_triggered(void)
{
    /* Vérifie que l'identifiant de l'élément séléctionné dans la liste des
     * courses est bien celui d'un tour. A savoir un QMap<QString, QVariant> */
    if(!this->raceViewItemidentifier.canConvert< TrackIdentifier >())
        return;

    // Récupère l'identifiant du tour sélectionné
    TrackIdentifier trackIdentifier =
            this->raceViewItemidentifier.value< TrackIdentifier >();

    this->exportLapDataToCSV(trackIdentifier, 0, 10000);
}

void MainWindow::on_actionRaceViewRemoveLap_triggered(void)
{
    /* Vérifie que l'identifiant de l'élément séléctionné dans la liste des
     * courses est bien celui d'un tour. A savoir un QMap<QString, QVariant> */
    if(!this->raceViewItemidentifier.canConvert< TrackIdentifier >())
    {
        qDebug() << "Impossible de convertir le raceViewItemidentifier en "
                    "TrackIdentifier. Le QVariant contient surement "
                    "une valeur d'un autre type";
        return;
    }

    // Récupère l'identifiant du tour sélectionné
    TrackIdentifier trackIdentifier =
            this->raceViewItemidentifier.value< TrackIdentifier >();

    this->removeTrackFromAllView(trackIdentifier);
}

void MainWindow::on_actionRaceViewDeleteRace_triggered(void)
{
    /* Vérifie que l'identifiant de l'élément séléctionné dans la liste des
     * courses est bien celui d'une course. A savoir un entier */
    if(!this->raceViewItemidentifier.canConvert<int>())
    {
        qDebug() << "Impossible de convertir le raceViewItemidentifier en "
                    "entier. Le QVariant contient surement une valeur d'un "
                    "autre type";
        return;
    }

    this->deleteRace(this->raceViewItemidentifier.value<int>());
}

void MainWindow::on_actionRaceViewDeleteRacesAtSpecificDate_triggered()
{
    /* Vérifie que l'identifiant de l'élément séléctionné dans la liste des
     * courses est bien celui d'une date. A savoir un QDate */
    if(!this->raceViewItemidentifier.canConvert<QDate>())
    {
        qDebug() << "Impossible de convertir le raceViewItemidentifier en "
                    "date. Le QVariant contient surement une valeur d'un "
                    "autre type";
        return;
    }

    // Récupère la date et le nom de la compétition
    QDate date = this->raceViewItemidentifier.value<QDate>();

    QSqlQuery raceIdAtSpecificDate("SELECT race.id "
                                   "FROM race, competition "
                                   "WHERE race.ref_compet = competition.name "
                                   "AND race.date like ? "
                                   "AND competition.name like ?");
    raceIdAtSpecificDate.addBindValue(date);
    raceIdAtSpecificDate.addBindValue(this->currentCompetition);

    if (!raceIdAtSpecificDate.exec())
    {
        QMessageBox::warning(this, tr("Impossible de supprimer les courses"),
                             raceIdAtSpecificDate.lastError().text());
        return;
    }

    // Supprime toutes les courses à la date donnée
    QVariantList racesId;
    while(raceIdAtSpecificDate.next())
        racesId << raceIdAtSpecificDate.value(0).toInt();

    this->deleteRaces(racesId);
}

void MainWindow::on_actionDeleteCurrentCompetition_triggered(void)
{
    if (this->currentCompetition.isEmpty())
        return;

    // Confirmation
    int confirmation = QMessageBox::warning(
                this, tr("Confirmation de suppression"),
                tr("Êtes vous sur de vouloir supprimer la compétition ")
                + this->currentCompetition + "?",
                QMessageBox::Yes, QMessageBox::No);

    if(confirmation != QMessageBox::Yes)
        return;

    QSqlQuery deleteCompetition;
    deleteCompetition.prepare("DELETE FROM competition WHERE name LIKE ?");
    deleteCompetition.addBindValue(this->currentCompetition);

    QSqlDatabase::database().driver()->beginTransaction();

    if(!deleteCompetition.exec())
    {
        QSqlDatabase::database().driver()->rollbackTransaction();

        QMessageBox::information(this, tr("Erreur Suppression"),
                                 tr("Impossible de supprimer la compétition ")
                                 + this->currentCompetition +
                                 deleteCompetition.lastError().text());
        return;
    }

    QSqlDatabase::database().driver()->commitTransaction();

    // Met à jour le combobox avec toutes les compétition et supprimera tout ce qui est affiché
    this->competitionNameModel->select();
    this->ui->sectorView->setVisible(false);
}

void MainWindow::on_actionNewProject_triggered(void)
{
    QString dbFilePath = QFileDialog::getSaveFileName(
          this, tr("Choisissez l'endroit ou sauvegarder les données du projet"),
                QDir::homePath(), tr("Projet EcoManager (*.db)"));

    if(dbFilePath.isEmpty()) // User canceled
        return;

    this->updateDataBase(dbFilePath, DataBaseManager::createDataBase);
}

void MainWindow::on_actionOpenProject_triggered(void)
{
    QString dbFilePath = QFileDialog::getOpenFileName(
                this, tr("Ouvrir un projet EcoMotion"), QDir::homePath(),
                tr("Projet EcoManager (*.db)"));

    if (dbFilePath.isEmpty()) // User canceled
        return;

    this->updateDataBase(dbFilePath, DataBaseManager::openExistingDataBase);
}

void MainWindow::on_actionCompter_le_nombre_de_tours_triggered(void)
{
    QSqlQuery cptTours2("SELECT COUNT(*) FROM LAP");
    if (!cptTours2.exec())
    {
        qDebug() << "Erreur compte tours : " << cptTours2.lastError();
        return;
    }

    if (cptTours2.next())
        qDebug() << "Nombre de tours dans la db = " << cptTours2.value(0).toString();
    else
        qDebug() << "La requete n'a retourné aucun résultat ...";
}

void MainWindow::on_actionCompter_le_nombre_de_courses_triggered(void)
{
    QSqlQuery cptRace("SELECT COUNT(*) FROM race");
    if (!cptRace.exec())
    {
        qDebug() << "Erreur compte race : " << cptRace.lastError();
        return;
    }

    if (cptRace.next())
        qDebug() << "Nombre de race dans la db = " << cptRace.value(0).toString();
    else
        qDebug() << "La requete n'a retourné aucun résultat ...";
}

void MainWindow::on_actionPRAGMA_foreign_keys_triggered(void)
{
    QSqlQuery query("PRAGMA foreign_keys");
    if (!query.exec())
    {
        qDebug() << "Erreur PRAGMA foreign_keys : " << query.lastError();
        return;
    }

    if (query.next())
        qDebug() << "PRAGMA foreign_keys = " << query.value(0).toBool();
    else
        qDebug() << "La requete n'a retourné aucun résultat ...";

    // PRAGMA foreign_keys = ON;
    QSqlQuery set("PRAGMA foreign_keys = ON");
    if (!set.exec())
    {
        qDebug() << "Erreur PRAGMA foreign_keys = ON : " << set.lastError();
        return;
    }

    if (!query.exec())
    {
        qDebug() << "Erreur PRAGMA foreign_keys : " << query.lastError();
        return;
    }

    if (query.next())
        qDebug() << "PRAGMA foreign_keys = " << query.value(0).toBool();
    else
        qDebug() << "La requete n'a retourné aucun résultat ...";
}

void MainWindow::removeTrackFromAllView(TrackIdentifier const& trackId)
{
    if (this->mapFrame->scene()->removeTrack(trackId))
        qDebug() << "mapping Supprimé !!!";

    this->distancePlotFrame->deleteCurve(trackId);
    this->timePlotFrame->deleteCurve(trackId);

    this->currentTracksDisplayed.removeOne(trackId);
}

void MainWindow::on_actionListing_des_courses_triggered(void)
{
    QSqlQuery set("SELECT * FROM RACE");
    if (!set.exec())
    {
        qDebug() << "Erreur listing : " << set.lastError();
        return;
    }

    while(set.next())
    {
        qDebug() << "-----------------------------------";
        qDebug() << "id = " << set.value(0).toInt();
        qDebug() << "num = " << set.value(1).toInt();
    }
}

void MainWindow::on_actionListing_des_competitions_triggered(void)
{
    QSqlQuery listing("SELECT name FROM COMPETITION");
    if (!listing.exec())
    {
        qDebug() << "Erreur listing competition : " << listing.lastError().text();
    }

    qDebug() << "----------- COMPETITIONS ------------------------";
    while(listing.next())
        qDebug() << listing.value(0).toString();
}

void MainWindow::on_actionCompter_tous_les_tuples_de_toutes_les_tables_triggered(void)
{
    QSqlQuery tableNames("select tbl_name from sqlite_master;");
    if(!tableNames.exec())
    {
        qDebug() << "Erreur lors du listing des tables : " << tableNames.lastError().text();
        return;
    }

    // Affichage du nom de toutes les tables
    qDebug() << "Toutes les tables de la base de données : ";
    qDebug() << "-----------------------------------------------";
    while(tableNames.next())
    {
        QString tableName = tableNames.value(0).toString();
        QSqlQuery cpt("SELECT COUNT(*) FROM " + tableName);

        if (!cpt.exec())
        {
            qDebug() << tableName << " impossible de compter le nombre de tuples : "
                     << cpt.lastError().text();
            return;
        }

        if (cpt.next())
            qDebug() << tableName << " a " << cpt.value(0).toInt() << " tuples";
    }
}

void MainWindow::loadCompetition(int index)
{
    qDebug() << "loadCompetition";

    this->currentCompetition = competitionNameModel->record(index).value(0).toString();

    // Clear all tracks information of each view
    this->on_actionClearAllData_triggered();

    // Load races information
    this->reloadRaceView();
}

void MainWindow::removeSector(const QString &competitionName, int sectorNum)
{
    QSqlDatabase::database().driver()->beginTransaction();

    QSqlQuery delQuery("delete from sector where ref_compet = ? and num = ?");
    delQuery.addBindValue(competitionName);
    delQuery.addBindValue(sectorNum);

    if (delQuery.exec())
    {
        qDebug() << "sector deleted.";

        QSqlQuery updateNumSect("update SECTOR set num = num - 1 where ref_compet = ? and num > ?");
        updateNumSect.addBindValue(competitionName);
        updateNumSect.addBindValue(sectorNum);

        if (!updateNumSect.exec())
        {
            qWarning() << updateNumSect.lastError();
            QSqlDatabase::database().driver()->rollbackTransaction();
        }
        else
        {
            QSqlDatabase::database().driver()->commitTransaction();
            qDebug() << "sector effectively deleted.";
        }
    }
    else
    {
        qWarning() << "Unable to delete sector" << delQuery.lastError();
        QSqlDatabase::database().driver()->rollbackTransaction();
    }

    this->sectorModel->select();
    this->ui->sectorView->setVisible(this->sectorModel->rowCount() > 0);
}

void MainWindow::addSector(QString competName, int sectNum,
                           IndexedPosition firstCoord, IndexedPosition lastCoord)
{
    QSqlDatabase::database().driver()->beginTransaction();

    QSqlQuery updateNumSect("update SECTOR set num = num + 1 where ref_compet = ? and num >= ?");
    updateNumSect.addBindValue(competName);
    updateNumSect.addBindValue(sectNum);

    if (!updateNumSect.exec())
    {
        QSqlDatabase::database().driver()->rollbackTransaction();
    }
    else
    {
        QSqlQuery insertQuery("insert into SECTOR (num, ref_compet, start_pos, end_pos) values (?, ?, ?, ?)");
        insertQuery.addBindValue(sectNum);
        insertQuery.addBindValue(competName);
        insertQuery.addBindValue(firstCoord.index());
        insertQuery.addBindValue(lastCoord.index());

        if (insertQuery.exec())
        {
            QSqlDatabase::database().driver()->commitTransaction();
        }
        else
        {
            qWarning() << "Unable to insert new sector after : " << insertQuery.lastError();
            QSqlDatabase::database().driver()->rollbackTransaction();
        }
    }

    this->sectorModel->select();
    this->ui->sectorView->setVisible(true);
}

void MainWindow::updateSector(QString competName, int sectNum,
                          IndexedPosition firstCoord, IndexedPosition lastCoord)
{
    QSqlQuery updateBoundaries("update SECTOR set start_pos = ?, end_pos = ? where ref_compet = ? and num = ?");
    updateBoundaries.addBindValue(firstCoord.index());
    updateBoundaries.addBindValue(lastCoord.index());
    updateBoundaries.addBindValue(competName);
    updateBoundaries.addBindValue(sectNum);

    if (!updateBoundaries.exec())
        qWarning() << updateBoundaries.lastQuery() << updateBoundaries.lastError();
}

void MainWindow::displayLapInformation(float timeValue, const QVariant &trackId)
{
    this->displayLapInformation(timeValue, timeValue, trackId);
}

void MainWindow::displayLapInformation(
        float lowerTimeValue, float upperTimeValue, const QVariant &trackId)
{
    if(!trackId.canConvert< TrackIdentifier >())
    {
        QMessageBox::warning(this, tr("Une erreur est survenue"),
                             tr("Impossible d'identifier le tour"));
        return;
    }

    // Get the race number and the lap number from the trackId
    TrackIdentifier trackIdentifier =
            qvariant_cast< TrackIdentifier >(trackId);
    int ref_race = trackIdentifier["race"].toInt();
    int ref_lap  = trackIdentifier["lap"].toInt();

    // Calcul de tous les paramètres (vitesses, distances, ...)
    QList< QList<QVariant> > lapDataList;
    if (!this->getAllDataFromSpeed(trackIdentifier, lowerTimeValue,
                                   upperTimeValue, lapDataList))
        return;

    qDebug() << "On a bien récupéré toutes les données calculées : " << lapDataList.count();

    // Ajout d'une première donnée vide à chaque élément
    for(int i(0); i < lapDataList.count(); ++i)
        lapDataList[i].insert(0, QVariant());

    // Ajout des données dans le tableau
    this->raceInformationTableModel->addMultipleRaceInformation(
                ref_race, ref_lap, lapDataList);

    this->ui->raceTable->expandAll();
}

void MainWindow::deleteRace(int raceId)
{
    /* ---------------------------------------------------------------------- *
     *                       Delete race from data base                       *
     * ---------------------------------------------------------------------- */
    QSqlQuery deleteRaceQuery("DELETE FROM RACE WHERE id = ?");
    deleteRaceQuery.addBindValue(raceId);

    qDebug() << "Requete de suppression = " << deleteRaceQuery.lastQuery();
    qDebug() << "id de la course = " << raceId;

    QSqlDatabase::database().driver()->beginTransaction();

    if (!deleteRaceQuery.exec())
    {
        // Restore data
        QSqlDatabase::database().driver()->rollbackTransaction();

        QMessageBox::warning(this, tr("Impossible de supprimer la course "),
                             deleteRaceQuery.lastError().text());
        return;
    }

    QSqlDatabase::database().driver()->commitTransaction();

    /* ---------------------------------------------------------------------- *
     *                          Update the race list                          *
     * ---------------------------------------------------------------------- */
    this->reloadRaceView();

    /* ---------------------------------------------------------------------- *
     *     Supprime les éventuels tours de la course qui seraient affichés    *
     * ---------------------------------------------------------------------- */

//    for (int i(0); i < this->currentTracksDisplayed.count(); ++i)
//        if (this->currentTracksDisplayed.at(i)["race"].toInt() == raceId)
//            this->removeTrackFromAllView(this->currentTracksDisplayed.at(i--));

    foreach (TrackIdentifier trackId, this->currentTracksDisplayed)
        if(trackId["race"].toInt() == raceId)
            this->removeTrackFromAllView(trackId);

//    this->mapFrame->scene()->clearSectors();
//    this->loadSectors(this->currentCompetition);
}

void MainWindow::deleteRaces(QVariantList listRaceId)
{
    /* ---------------------------------------------------------------------- *
     *                       Delete races from data base                       *
     * ---------------------------------------------------------------------- */
    QSqlQuery deleteRacesQuery;
    deleteRacesQuery.prepare("DELETE FROM race WHERE id = ?");
    deleteRacesQuery.addBindValue(listRaceId);

    QSqlDatabase::database().driver()->beginTransaction();

    if (!deleteRacesQuery.execBatch())
    {
        // Restore data
        QSqlDatabase::database().driver()->rollbackTransaction();

        QMessageBox::warning(this, tr("Impossible de supprimer la course "),
                             deleteRacesQuery.lastQuery() +
                             deleteRacesQuery.lastError().text());
        return;
    }

    QSqlDatabase::database().driver()->commitTransaction();

    /* ---------------------------------------------------------------------- *
     *                          Update the race list                          *
     * ---------------------------------------------------------------------- */
    this->reloadRaceView();

    /* ---------------------------------------------------------------------- *
     *     Supprime les éventuels tours de la course qui seraient affichés    *
     * ---------------------------------------------------------------------- */

    foreach (TrackIdentifier trackId, this->currentTracksDisplayed)
        if (listRaceId.contains(trackId["race"]))
            this->removeTrackFromAllView(trackId);

//    for (int i(0); i < this->currentTracksDisplayed.count(); ++i)
//        if (this->currentTracksDisplayed.at(i)["race"].toInt() == raceId)
//            this->removeTrackFromAllView(this->currentTracksDisplayed.at(i--));

//    this->mapFrame->scene()->clearSectors();
//    this->loadSectors(this->currentCompetition);
}

void MainWindow::centerOnScreen(void)
{
    QDesktopWidget screen;

    QRect screenGeom = screen.screenGeometry(this);

    int screenCenterX = screenGeom.center().x();
    int screenCenterY = screenGeom.center().y();

    this->move(screenCenterX - width () / 2, screenCenterY - height() / 2);
}

void MainWindow::createRaceView(void)
{
    // Mainly developed with Qt Designer

    this->ui->raceView->resizeColumnToContents(0);
}

void MainWindow::createToolsBar(void)
{
    // Mainly developed with Qt Designer

    this->competitionNameModel = new QSqlTableModel(this);
    this->competitionNameModel->setTable("COMPETITION");
    //this->competitionNameModel->removeRows(1, 2);
    this->competitionNameModel->removeRow(2); // Lieu

    // Create a comboBox used to selecting a competition
    this->competitionBox = new QComboBox();
    this->competitionBox->setEditable(false);
    this->competitionBox->setModel(this->competitionNameModel);
    this->competitionBox->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Maximum);

    // Add the comboBox to the toolBar of the MainWindow
    this->ui->mainToolBar->insertWidget(this->ui->actionDelimitingSectors,
                                        this->competitionBox);

    QObject::connect(this->competitionBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(loadCompetition(int)));

    this->competitionNameModel->select();
}

void MainWindow::createMapZone(void)
{
    // Create the map frame (the map view is included)
    this->mapFrame = new MapFrame(this);
    this->mapFrame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    // Manage the sector view
    this->ui->sectorView->setVisible(false);
    this->ui->sectorView->installEventFilter(this);

    // Manage the map splitter
    this->ui->mapSplitter->insertWidget(0, this->mapFrame);
    this->ui->mapSplitter->setStretchFactor(0, 3);

    // Manage sector model
    this->sectorModel = NULL;
}

void MainWindow::createPlotsZone(void)
{
    /* ---------------------------------------------------------------------- *
     *                       Create distance plot frame                       *
     * ---------------------------------------------------------------------- */
    this->distancePlotFrame = new AdvancedPlot(
                tr("Distance - Vitesse"), 6, this);

    this->distancePlotFrame->setAxisTitle(Plot::xBottom, tr("Distance (m)"));
    this->distancePlotFrame->setAxisTitle(Plot::yLeft, tr("Vitesse (km\\h)"));
    this->ui->distancePlotLayout->addWidget(this->distancePlotFrame);

    // Connect plot signals to slots
    connect(this->distancePlotFrame, SIGNAL(legendChecked(QwtPlotItem*, bool)),
            this,  SLOT(setPlotCurveVisibile(QwtPlotItem*, bool)));
    connect(this->distancePlotFrame,
            SIGNAL(legendRightClicked(const QwtPlotItem*,QPoint)),
            this, SLOT(showLegendContextMenu(const QwtPlotItem*,QPoint)));

    // Settings management configuration
    this->distancePlotFrame->setObjectName("DistancePlot");
    this->plots.append(this->distancePlotFrame);

    /* ---------------------------------------------------------------------- *
     *                         Create time plot frame                         *
     * ---------------------------------------------------------------------- */

    this->timePlotFrame = new AdvancedPlot(tr("Temps - Vitesse"), 6, this);
    this->timePlotFrame->setAxisTitle(Plot::xBottom, tr("Temps (s)"));
    this->timePlotFrame->setAxisTitle(Plot::yLeft, tr("Vitesse (km\\h)"));
    this->ui->timePlotLayout->addWidget(this->timePlotFrame);

    // Connect plot signals to slots
    connect(this->timePlotFrame, SIGNAL(legendChecked(QwtPlotItem*, bool)),
            this,  SLOT(setPlotCurveVisibile(QwtPlotItem*, bool)));
    connect(this->timePlotFrame,
            SIGNAL(legendRightClicked(const QwtPlotItem*,QPoint)),
            this, SLOT(showLegendContextMenu(const QwtPlotItem*,QPoint)));

    // Settings management configuration
    this->timePlotFrame->setObjectName("TimePlot");
    this->plots.append(this->timePlotFrame);

    /* ---------------------------------------------------------------------- *
     *                         Create Megasquirt plot                         *
     * ---------------------------------------------------------------------- */

    this->megasquirtDataPlot = new AdvancedPlot(tr("Données du Megasquirt"),
                                                6, this);
    this->megasquirtDataPlot->setAxisTitle(Plot::xBottom, tr("Temps (s)"));
    this->ui->megaSquirtSplitter->addWidget(this->megasquirtDataPlot);

    // Connect plot signals to slots
    connect(this->megasquirtDataPlot, SIGNAL(legendChecked(QwtPlotItem*, bool)),
            this,  SLOT(setPlotCurveVisibile(QwtPlotItem*, bool)));
    connect(this->megasquirtDataPlot,
            SIGNAL(legendRightClicked(const QwtPlotItem*,QPoint)),
            this, SLOT(showLegendContextMenu(const QwtPlotItem*,QPoint)));

    // Settings management configuration
    this->megasquirtDataPlot->setObjectName("MegasquirtDataPlot");
    this->plots.append(this->megasquirtDataPlot);

    // Get Megasquirt parameters list
    MSManager megasquirtManager;
    this->ui->megaSquirtComboBox->addItems(megasquirtManager.fields());
}

void MainWindow::createRaceTable(void)
{
    // Create the model for the table of laps information
    QStringList headers;
    headers << tr("Course") << tr("Tps(ms)") << tr("Tps(s)") << tr("Dist(m)")
            << tr("v(km\\h)") << tr("Acc (m\\s2)") << tr("RPM") << tr("PW");
    this->raceInformationTableModel = new LapInformationTreeModel(headers); //this->raceInformationTableModel = new TreeLapInformationModel(headers);

    /* Use a proxy model to manage background color of each row and manage
     * how the data for Race and lap number are displayed */
    LapInformationProxyModel* wrapper = new LapInformationProxyModel(this);
    wrapper->setSourceModel(this->raceInformationTableModel);

    // Apply the model to the table and change the selection mode
    //this->ui->raceTable->setModel(this->raceInformationTableModel);
    this->ui->raceTable->setModel(wrapper);
    this->ui->raceTable->setSelectionMode(QAbstractItemView::MultiSelection);
    this->ui->raceTable->setAlternatingRowColors(true); // Can be done with stylesheet, proxyModel or the mainModel
}

void MainWindow::readSettings(void)
{
    QSettings settings;

    // Restore plots settings
    foreach (Plot* plot, this->plots)
    {
        settings.beginGroup(plot->objectName());
        plot->setGridVisible(
                    settings.value("isGridVisible", true).toBool());
        plot->setCrossLineVisible(
                    settings.value("isCrossLineVisible", false).toBool());
        plot->setLabelPositionVisible(
                    settings.value("isLabelPositionVisible", true).toBool());
        settings.endGroup();
    }

    // Restore MainWindow settings
    this->readLayoutSettings("MainWindow");
}

void MainWindow::writeSettings(void) const
{
    QSettings settings;

    // Save plots settings
    foreach (Plot* plot, this->plots)
    {
        settings.beginGroup(plot->objectName());
        settings.setValue("isGridVisible",
                          plot->isGridVisible());
        settings.setValue("isCrossLineVisible",
                          plot->isCrossLineVisible());
        settings.setValue("isLabelPositionVisible",
                          plot->isLabelPositionVisible());
        settings.endGroup();
    }

    // Save MainWindow settings
    this->writeLayoutSettings("MainWindow");
}

void MainWindow::readLayoutSettings(const QString& settingsGroup)
{
    QSettings settings;

    settings.beginGroup(settingsGroup);

    /* Contourne le bug non résolu par Qt de la restauration de la géométrie
     * d'une fenetre maximisée alors qu'elle est déjà maximisée */
    if (settings.value("isMaximized").toBool())
        this->showMaximized();
    else
        this->restoreGeometry(settings.value("geometry").toByteArray());

    this->ui->mainSplitter->restoreState(
                settings.value("mainSplitter").toByteArray());
    this->ui->MapPlotAndRaceSplitter->restoreState(
                settings.value("MapPlotAndRaceSplitter").toByteArray());
    this->ui->MapPlotSplitter->restoreState(
                settings.value("MapPlotSplitter").toByteArray());
    this->ui->megaSquirtSplitter->restoreState(
                settings.value("megaSquirtSplitter").toByteArray());
    this->ui->plotsTabWidget->setCurrentIndex(
                settings.value("plotsTabWidgetCurrentIndex", 0).toInt());

    settings.endGroup();
}

void MainWindow::writeLayoutSettings(const QString& settingsGroup) const
{
    QSettings settings;

    settings.beginGroup(settingsGroup);

    settings.setValue("isMaximized",
                      this->isMaximized());
    settings.setValue("geometry",
                      this->saveGeometry());
    settings.setValue("mainSplitter",
                      this->ui->mainSplitter->saveState());
    settings.setValue("MapPlotAndRaceSplitter",
                      this->ui->MapPlotAndRaceSplitter->saveState());
    settings.setValue("MapPlotSplitter",
                      this->ui->MapPlotSplitter->saveState());
    settings.setValue("megaSquirtSplitter",
                      this->ui->megaSquirtSplitter->saveState());
    settings.setValue("plotsTabWidgetCurrentIndex",
                      this->ui->plotsTabWidget->currentIndex());

    settings.endGroup();
}

void MainWindow::displayDataLap(void)
{
    /* Vérifie que l'identifiant de l'élément séléctionné dans la liste des
     * courses est bien celui d'un tour. A savoir un QMap<QString, QVariant> */
    if(!this->raceViewItemidentifier.canConvert< TrackIdentifier >())
        return;

    // Get the track id for the selected lap
    TrackIdentifier trackIdentifier =
            this->raceViewItemidentifier.value< TrackIdentifier >();

    // Check if the track is already displayed
    if (this->currentTracksDisplayed.contains(trackIdentifier))
    {
        QMessageBox::warning(
                    this, tr("Opération impossible"),
                    tr("Ce tour est déjà affiché dans les différentes vues"));
        return;
    }

    qDebug("tour pas encore affiché");
    this->currentTracksDisplayed.append(trackIdentifier);

    int ref_race = trackIdentifier["race"].toInt();
    int ref_lap  = trackIdentifier["lap"].toInt();

    /* ---------------------------------------------------------------------- *
     *                           Populate map scene                           *
     * ---------------------------------------------------------------------- */
    QSqlQuery posQuery("select longitude, latitude, timestamp from POSITION where ref_lap_race = ? and ref_lap_num = ? order by timestamp");
    posQuery.addBindValue(ref_race);
    posQuery.addBindValue(ref_lap);

    if (posQuery.exec())
    {
        QVector<QPointF> pos;
        QVector<float> indexValues;

        while (posQuery.next())
        {
            GeoCoordinate tmp;
            tmp.setLongitude(posQuery.value(0).toFloat());
            tmp.setLatitude(posQuery.value(1).toFloat());

            //see projection method doc for usage purpose
            pos.append(tmp.projection());
            indexValues << posQuery.value(2).toFloat() / 1000;
        }

        qDebug() << posQuery.lastQuery();
        //qDebug() <<  ref_race << curIndex.row() << pos.size();
        this->mapFrame->scene()->addTrack(pos, indexValues, trackIdentifier);
    }

    // If a sampling lap has already be defined, just load it in the view
    if (!this->mapFrame->scene()->hasSectors())
        this->loadSectors(this->currentCompetition);

    /* ---------------------------------------------------------------------- *
     *                          Populate pots scene                           *
     * ---------------------------------------------------------------------- */
    try
    {
        QVariantList param;
        param << ref_race << ref_lap;
        QSqlQuery test = DataBaseManager::execQuery(
                    "SELECT timestamp, speed, distance "
                    "FROM datarace "
                    "WHERE ref_lap_race = ? AND ref_lap_num = ? ORDER BY timestamp",
                    param);

        QVector<QPointF> distanceSpeedPoints;
        QVector<QPointF> timeSpeedPoints;

        while(test.next())
        {
            distanceSpeedPoints << QPointF(test.value(2).toReal(),
                                           test.value(1).toReal());

            timeSpeedPoints << QPointF(test.value(0).toReal() / 1000,
                                       test.value(1).toReal());
        }

        TrackPlotCurve* curve;
        curve = this->distancePlotFrame->addCurve(
                    distanceSpeedPoints, trackIdentifier);
        this->setPlotCurveVisibile(curve, true);

        curve = this->timePlotFrame->addCurve(timeSpeedPoints, trackIdentifier);
        this->setPlotCurveVisibile(curve, true);
    }
    catch(QException const& ex)
    {
        QMessageBox::warning(
                    this, tr("Impossible d'afficher les courbes"), ex.what());
    }
}

void MainWindow::connectSignals(void)
{
    // Map frame/scene
    connect(this->mapFrame->scene(), SIGNAL(sectorRemoved(QString,int)),
            this, SLOT(removeSector(QString,int)));
    connect(this->mapFrame->scene(), SIGNAL(sectorAdded(QString,int,IndexedPosition,IndexedPosition)),
            this, SLOT(addSector(QString,int,IndexedPosition,IndexedPosition)));
    connect(this->mapFrame->scene(), SIGNAL(sectorUpdated(QString,int,IndexedPosition,IndexedPosition)),
            this, SLOT(updateSector(QString,int,IndexedPosition,IndexedPosition)));

//    connect(this->mapFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
//            this->distancePlotFrame->scene(), SLOT(highlightPoints(float,QVariant)));

    connect(this->mapFrame->scene(), SIGNAL(pointSelected(float,QVariant)), //    connect(this->mapFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
            this->timePlotFrame, SLOT(highlightSector(float,QVariant)));    //            this->timePlotFrame->scene(), SLOT(highlightPoints(float,QVariant)));

//    connect(this->mapFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
//            this->distancePlotFrame->scene(), SLOT(highlightSector(float,float,QVariant)));


    connect(this->mapFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)), //    connect(this->mapFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this->timePlotFrame, SLOT(highlightSector(float,float,QVariant)));      //            this->timePlotFrame->scene(), SLOT(highlightSector(float,float,QVariant)));

    connect(this->mapFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this, SLOT(displayLapInformation(float,float,QVariant)));
//    connect(this->mapFrame->scene(), SIGNAL(selectionChanged()),
//            this->distancePlotFrame->scene(), SLOT(clearPlotSelection()));


    connect(this->mapFrame->scene(), SIGNAL(selectionChanged()), //    connect(this->mapFrame->scene(), SIGNAL(selectionChanged()),
            this->timePlotFrame, SLOT(clearSecondaryCurves())); //            this->timePlotFrame->scene(), SLOT(clearPlotSelection()));

    connect(this->mapFrame->scene(), SIGNAL(selectionChanged()),
            this, SLOT(on_actionLapDataEraseTable_triggered()));
    connect(this->mapFrame, SIGNAL(clearTracks()),
            this, SLOT(on_actionClearAllData_triggered()));

    // Distance plot frame/scene
    //connect(this->distancePlotFrame, SIGNAL(selectionChanged()),
            //this->mapFrame->scene(), SLOT(clearSceneSelection()));
//    connect(this->distancePlotFrame->scene(), SIGNAL(selectionChanged()),
//            this, SLOT(on_actionLapDataEraseTable_triggered()));
//    connect(this->distancePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
//            this->mapFrame->scene(), SLOT(highlightPoint(float,QVariant)));
//    connect(this->distancePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
//            this, SLOT(displayLapInformation(float,QVariant)));
//    connect(this->distancePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
//            this->mapFrame->scene(), SLOT(highlightSector(float,float,QVariant)));
//    connect(this->distancePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
//            this, SLOT(displayLapInformation(float,float,QVariant)));
//    connect(this->distancePlotFrame, SIGNAL(clear()),
//            this, SLOT(on_actionClearAllData_triggered()));

    // Time plot frame/scene
    //connect(this->timePlotFrame, SIGNAL(selectionChanged()),
            //this->mapFrame->scene(), SLOT(clearSceneSelection()));
//    connect(this->timePlotFrame->scene(), SIGNAL(selectionChanged()),
//            this, SLOT(on_actionLapDataEraseTable_triggered()));
//    connect(this->timePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
//            this->mapFrame->scene(), SLOT(highlightPoint(float,QVariant)));
//    connect(this->timePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
//            this, SLOT(displayLapInformation(float,QVariant)));
//    connect(this->timePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
//            this->mapFrame->scene(), SLOT(highlightSector(float,float,QVariant)));
//    connect(this->timePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
//            this, SLOT(displayLapInformation(float,float,QVariant)));
//    connect(this->timePlotFrame, SIGNAL(clear()),
//            this, SLOT(on_actionClearAllData_triggered()));

    // Test de la connection des graphiques Qwt
    connect(this->megasquirtDataPlot, SIGNAL(intervalSelected(float,float,QVariant)),
            this->mapFrame->scene(), SLOT(highlightOnlySector(float,float,QVariant)));
}

void MainWindow::reloadRaceView(void)
{
    qDebug() << "reloadRaceView";

    QSqlQueryModel* model = new QSqlQueryModel(this);
    QSqlQuery getAllLaps;
    getAllLaps.prepare("select date, race.num, lap.num, race.id, lap.num "
                       "from COMPETITION, RACE, LAP "
                       "where COMPETITION.name = ? and COMPETITION.name = RACE.ref_compet and RACE.id = LAP.ref_race "
                       "order by RACE.date, RACE.num, LAP.num");
    getAllLaps.addBindValue(this->currentCompetition);

    if (!getAllLaps.exec())
    {
        QMessageBox::information(
                    this, tr("Impossible de charger la compétition ")
                    + this->currentCompetition, getAllLaps.lastError().text());
        return;
    }

    model->setQuery(getAllLaps);

    this->competitionModel = new GroupingTreeModel(this);
    QList<int> cols;
    cols << 0 << 1; // ce par quoi on va grouper les éléments dans la vue (première colonne)
    this->competitionModel->setSourceModel(model, cols);

    CompetitionProxyModel* wrapper = new CompetitionProxyModel(this);
    wrapper->setSourceModel(this->competitionModel);

    QItemSelectionModel* oldSelectionModel = this->ui->raceView->selectionModel();
    if (oldSelectionModel != NULL)
        delete oldSelectionModel;

    this->ui->raceView->setModel(wrapper);
    this->ui->raceView->expandAll();
    this->ui->raceView->resizeColumnToContents(0);

    /* Do not show race.id or lap.num through view*/
    this->ui->raceView->setColumnHidden(1, true);
    this->ui->raceView->setColumnHidden(2, true);

//    connect(this->ui->raceView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(competitionSelection(QItemSelection,QItemSelection)));
}

void MainWindow::loadSectors(const QString &competitionName)
{
    this->sectorModel = new QSqlTableModel(this);
    this->sectorModel->setTable("SECTOR");
    this->sectorModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    this->sectorModel->setFilter(QString("ref_compet = '%1'").arg(competitionName));
    this->sectorModel->setSort(this->sectorModel->fieldIndex("num"), Qt::AscendingOrder);
    this->sectorModel->setHeaderData(3, Qt::Horizontal, tr("Vmin (km/h)"));
    this->sectorModel->setHeaderData(4, Qt::Horizontal, tr("Vmax (km/h)"));
    this->sectorModel->select();

    ColorizerProxyModel* coloredModel = new ColorizerProxyModel(6, this);
    coloredModel->setSourceModel(this->sectorModel);
    coloredModel->setIndexColumn(1);
    this->ui->sectorView->setModel(coloredModel);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("id"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("num"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("ref_compet"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("start_pos"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("end_pos"), true);
    this->ui->sectorView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QSqlQuery posQuery;
    posQuery.prepare("select longitude, latitude, id from POSITION where id >= ? and id <= ?");
    int currentSector = 0;
    int startPosInd = this->sectorModel->fieldIndex("start_pos");
    int endPosInd = this->sectorModel->fieldIndex("end_pos");

    qDebug() << "sectors nb ; " << this->sectorModel->rowCount()  ;
    while (currentSector < this->sectorModel->rowCount())
    {
        posQuery.bindValue(0, this->sectorModel->record(currentSector).value(startPosInd));
        posQuery.bindValue(1, this->sectorModel->record(currentSector).value(endPosInd));

        if (posQuery.exec())
        {
            QVector<IndexedPosition> sectorPoints;

            while (posQuery.next())
            {
                GeoCoordinate tmp;
                tmp.setLongitude(posQuery.value(0).toFloat());
                tmp.setLatitude(posQuery.value(1).toFloat());
                sectorPoints.append(IndexedPosition(tmp.projection(), posQuery.value(2).toInt()));
            }

            if (! sectorPoints.isEmpty())
                this->mapFrame->scene()->addSector(sectorPoints, competitionName);
        }
        currentSector++;
    }

    this->ui->sectorView->setVisible(mapFrame->scene()->hasSectors());
}

void MainWindow::highlightPointInAllView(const QModelIndex &index)
{
    QModelIndex LapNumModelIndex;
    QModelIndex RaceNumModelIndex;

    // Get lap and race num
    LapNumModelIndex  = index.parent();
    RaceNumModelIndex = LapNumModelIndex.parent();

    TrackIdentifier trackId;
    trackId["race"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(RaceNumModelIndex.row(),
                                                       0)).toInt();

    trackId["lap"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(LapNumModelIndex.row(),
                                                 0, RaceNumModelIndex)).toInt();

    // ====================================================================================
    /* NOTE : Le code qui suit ne devrait pas etre là. Je devrais modifier le
     * model du tableau afin que chaque élément contienne le race number
     * (ou uniquement la ligne "course" du tableau. je n'ai donc pas encore
     * corrigé les problèmes du au fait que j'ai ajouté un champ dans le trackid
     */
    QSqlQuery query("SELECT num FROM race WHERE id = ?");
    query.addBindValue(trackId["race"]);

    if(!query.exec() || !query.next())
    {
        QMessageBox::warning(this, tr("Une erreur est survenue"),
                             tr("Impossible de récupérer le numéro de la course"));
        return;
    }

    trackId["race_num"] = query.value(0).toInt();
    qDebug() << "Numéro du tour = " << query.value(0).toInt();
    // ====================================================================================

    // Get the time in milliseconds for the selected item
    float time = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(
                    index.row(), 1, LapNumModelIndex)).toInt() / 1000.0;

    // Highlight point in all view
    this->mapFrame->scene()->highlightPoint(time, trackId);
//    this->timePlotFrame->scene()->highlightPoint(time, trackId);
//    this->distancePlotFrame->scene()->highlightPoint(time, trackId);
}

void MainWindow::updateDataBase(QString const& dbFilePath,
                               bool(*dataBaseAction)(QString const&))
{
    /* ---------------------------------------------------------------------- *
     * Plusieurs modèles (2) sont basé sur les tables, il faut donc les       *
     * supprimer en premier                                                   *
     * ---------------------------------------------------------------------- */
    this->ui->sectorView->setVisible(false);
    delete this->sectorModel;
    this->sectorModel = NULL;

    delete this->competitionNameModel;
    this->competitionNameModel = NULL;

    /* ---------------------------------------------------------------------- *
     *                      action sur la base de données                     *
     * ---------------------------------------------------------------------- */
    bool success = (*dataBaseAction)(dbFilePath);

    this->ui->actionImport->setVisible(success);
    this->ui->actionExportSectors->setVisible(success);
    this->on_actionClearAllData_triggered();

    if(!success)
        return;

    QFileInfo dbFile(QSqlDatabase::database().databaseName());
    this->setWindowTitle(tr("EcoManager - ") + dbFile.baseName());

    /* ---------------------------------------------------------------------- *
     *    Rétablir la liste des compétitions en fonction de la nouvelle db    *
     * ---------------------------------------------------------------------- */
    this->competitionNameModel = new QSqlTableModel(this);
    this->competitionNameModel->setTable("COMPETITION");
    //this->competitionNameModel->removeRows(1, 2);
    this->competitionNameModel->removeRow(1); // Lieu
    this->competitionBox->setModel(this->competitionNameModel);
    this->competitionNameModel->select();
    this->reloadRaceView();
}

double MainWindow::getCurrentCompetitionWheelPerimeter(void) const
{
    return this->competitionNameModel->index(
                this->competitionBox->currentIndex(), 2).data().toDouble();
}

bool MainWindow::getAllDataFromSpeed(
        const TrackIdentifier& trackId, float lowerTimeValue,
        float upperTimeValue, QList< QList<QVariant> >& data)
{
    // Get the race number and the lap number from the trackId
    int ref_race = trackId["race"].toInt();
    int ref_lap  = trackId["lap"].toInt();

    qDebug() << "Calcul de toutes les données pour le tour " << ref_lap << " de la course " << ref_race;
    qDebug() << "lower = " << lowerTimeValue;
    qDebug() << "upper = " << upperTimeValue;

    /* upperTimeValue passé en paramètre est exprimé en secondes mais les
     * timestamp sauvées dans la base de données sont en millisecondes */
    //int upperTimeStamp = upperTimeValue * 1000;

    // Récupérer les informations de temps et de vitesses
    QSqlQuery query;
    query.prepare("select timestamp, value from SPEED where timestamp <= ? and ref_lap_race = ? and ref_lap_num = ? order by timestamp");
    query.addBindValue(upperTimeValue * 1000);
    query.addBindValue(ref_race);
    query.addBindValue(ref_lap);

    /* If you only need to move forward through the results (e.g., by using next()),
     * you can use setForwardOnly(), which will save a significant amount of memory
     * overhead and improve performance on some databases. */
    query.setForwardOnly(true);

    if (!query.exec())
    {
        QString errorMsg(tr("Impossible de récupérer les données numériques "
                         "associées à votre sélection pour le tour ") +
                         QString::number(ref_lap) + tr(" de la course ") +
                         QString::number(ref_race));
        QMessageBox::warning(this, tr("Erreur de récupération de données"),
                             errorMsg);
        return false;
    }

    double wheelPerimeter(this->getCurrentCompetitionWheelPerimeter());
    double time(0),  lastTime(0);
    double speed(0), lastSpeed(0);
    double pos(wheelPerimeter),   lastPos(0);

    while(query.next())
    {
        lastTime  = time;
        lastSpeed = speed;
        lastPos   = pos;

        time  = query.value(0).toDouble() / 1000; // Le temps est sauvé en millisecondes dans la db et on le veut en secondes
        speed = query.value(1).toDouble();

        int multipleWheelPerimeter = ceil(((speed + lastSpeed) / (2 * 3.6)) * (time - lastTime)) / wheelPerimeter;
        qDebug() << "multipleWheelPerimeter = " << multipleWheelPerimeter;
        pos = lastPos + multipleWheelPerimeter * wheelPerimeter;

        // Données a afficher dans le tableau
        if(time >= lowerTimeValue)
        {
            qreal diff = (speed -lastSpeed) / 3.6; // vitesse en m/s
            qreal acc  = diff / (time - lastTime);

            QList<QVariant> lapData;
            lapData.append(time * 1000); // Tps (ms)
            lapData.append(time);        // Tps (s)
            lapData.append(pos);         // Dist (m)
            lapData.append(speed);       // V (km\h)
            lapData.append(qAbs(acc) > 2 ? "NS" : QString::number(acc)); // Acc (m\s²)

            // Ajout de la ligne de données à la liste
            data.append(lapData);
        }
    }

    qDebug() << "Nombre de données calculées = " << data.count();

    /* ---------------------------------------------------------------------- *
     *                Ajout des tours moteur et du pulse width                *
     * ---------------------------------------------------------------------- */

    QSqlQuery megasquirtQuery;
    megasquirtQuery.prepare(
                "SELECT timestamp, rpm, pulseWidth1 "
                "FROM megasquirt "
                "where timestamp >= ? "
                    "and timestamp <= ? "
                    "and ref_lap_race = ? "
                    "and ref_lap_num = ? "
                    "order by timestamp");
    megasquirtQuery.addBindValue(lowerTimeValue * 1000);
    megasquirtQuery.addBindValue(upperTimeValue * 1000);
    megasquirtQuery.addBindValue(ref_race);
    megasquirtQuery.addBindValue(ref_lap);

    if (!megasquirtQuery.exec())
    {
        QString errorMsg(tr("Impossible de récupérer les données moteur "
                         "associées à votre sélection pour le tour ") +
                         QString::number(ref_lap) + tr(" de la course ") +
                         QString::number(ref_race) +
                         megasquirtQuery.lastError().text());
        QMessageBox::warning(this, tr("Erreur de récupération de données"),
                             errorMsg);
        return false;
    }

    /* ---------------------------------------------------------------------- *
     *                   Ajout des données dans le tableau                    *
     * ---------------------------------------------------------------------- */

    int i(1);
    while(megasquirtQuery.next())
    {
        double MSTimeStamp = megasquirtQuery.value(0).toDouble();
        for(;i < data.count() && data[i].at(0).toDouble() < MSTimeStamp; ++i);

        data[i - 1] << megasquirtQuery.value(1)  // RPM
                    << megasquirtQuery.value(2); // PW
    }

    return true;
}

void MainWindow::exportLapDataToCSV(const TrackIdentifier &trackId,
                                    float lowerTimeValue, float upperTimeValue)
{
    // Demander le fichier ou sauver les données
    QString filepath = QFileDialog::getSaveFileName(
                this, tr("Choisir où sauvegarder les données du tour"),
                QDir::homePath(), tr("Fichier CSV (*.csv)"));

    if (filepath.isEmpty()) // User canceled
        return;

    // Supprime le fichier s'il existe
    QFile file(filepath);
    if(file.exists())
        file.remove();

    // Calcule toutes les données à mettre dans le fichier csv
    QList< QList<QVariant> > lapdata;
    if (!this->getAllDataFromSpeed(
                trackId, lowerTimeValue, upperTimeValue, lapdata))
    {
        QMessageBox::warning(this, tr("Impossible d'exporter les données"),
                             tr("Erreur lors de la récupération des données"));
        return;
    }

    // Converti toutes les données QVariant en QString
    QList<QCSVRow> data;
    foreach (QList<QVariant> dataRow, lapdata)
    {
        QCSVRow row;
        foreach (QVariant variantData, dataRow)
            row.append(variantData.toString());

        data.append(row);
    }

    // Sauvegarde toutes les données dans le fichier csv
    QCSVRow header;
    header << "Temps (ms)" << "Temps (s)" << "Distance (m)" << "V (km\\h)"
           << "Accélération (m\\s²)" << "RPM" << "PW";

    QCSVParser csvParser(filepath);
    csvParser.addRow(header);
    for(int i(0); i < data.count(); ++i)
        csvParser.addRow(data.at(i));
    csvParser.save();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Save the state of the mainWindow and its widgets
    this->writeSettings();

    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionExecuter_une_requete_triggered(void)
{
    QString stringQuery = QInputDialog::getText(
                this, "Executer une requete", "Entrez votre requete");

    if(stringQuery.isEmpty())
        return;

    try
    {
        QSqlQuery query(stringQuery);
        DataBaseManager::execTransaction(query);

        while (query.next())
            qDebug() << query.value(0).toString();
    }
    catch(QException const& ex)
    {
        QMessageBox::warning(this, "Erreur de requete", ex.what());
    }
}

void MainWindow::on_megasquirtAddCurvePushButton_clicked(void)
{
    try
    {
        // Check if a race has been selected
        if(!this->raceViewItemidentifier.canConvert<TrackIdentifier >())
            throw QException(tr("Vous devez sélectionner un tour dans la liste des courses"));

        TrackIdentifier trackIdentifier =
                this->raceViewItemidentifier.value< TrackIdentifier >();

        // Paramètre megasquirt pour lequel on veut tracer la courbe
        QString MSParameter = this->ui->megaSquirtComboBox->currentText();

        QVariantList param;
        param << trackIdentifier["race"] << trackIdentifier["lap"];
        QSqlQuery query = DataBaseManager::execQuery(
                "SELECT timestamp, " + MSParameter + " "
                "FROM megasquirt "
                "WHERE ref_lap_race = ? and ref_lap_num = ? order by timestamp",
                 param);

        // Création des points de la courbe
        QVector<QPointF> megasquirtCurvePoints;
        while(query.next())
            megasquirtCurvePoints << QPointF(query.value(0).toFloat() / 1000,
                                             query.value(1).toFloat());

        if(megasquirtCurvePoints.count() == 0)
            throw QException(tr("Aucune donnée ") + MSParameter +
                             tr(" associée au tour ") +
                             trackIdentifier["lap"].toString() +
                             tr(" de la course ") +
                             trackIdentifier["race_num"].toString());

        TrackPlotCurve* curve = this->megasquirtDataPlot->addCurve(
                    megasquirtCurvePoints, trackIdentifier, MSParameter);
        this->setPlotCurveVisibile(curve, true);
    }
    catch(QException const& ex)
    {
        QMessageBox::warning(
                    this, tr("Impossible d'ajouter la courbe"), ex.what());
    }

    // =========================================================================
    // =========================================================================
    // =========================================================================

//    if(!this->raceViewItemidentifier.canConvert< QMap<QString, QVariant> >())
//    {
//        QMessageBox::warning(this, tr("Action impossible"),
//                             tr("Vous devez sélectionner un tour dans la liste des courses"));
//        return;
//    }

//    TrackIdentifier trackIdentifier =
//            this->raceViewItemidentifier.value< QMap<QString, QVariant> >();

//    QSqlQuery query("SELECT timestamp, " + this->ui->megaSquirtComboBox->currentText() + " "
//                    "FROM megasquirt "
//                    "WHERE ref_lap_race = ? and ref_lap_num = ? order by timestamp");
//    query.addBindValue(trackIdentifier["race"].toInt());
//    query.addBindValue(trackIdentifier["lap"].toInt());

//    if (!query.exec())
//    {
//        QMessageBox::warning(this, tr("Impossible de tracer la courbe"),
//                             tr("Erreur de récupération des données megasquirt ")
//                             + query.lastError().text());
//        return;
//    }

//    // Création des points de la courbe
//    QVector<QPointF> megasquirtCurvePoints;
//    while(query.next())
//        megasquirtCurvePoints << QPointF(query.value(0).toFloat() / 1000,
//                                         query.value(1).toFloat());

//    if(megasquirtCurvePoints.count() == 0)
//    {
//        QMessageBox::information(
//                    this, tr("Action impossible"), tr("Aucune donnée ") +
//                    this->ui->megaSquirtComboBox->currentText() +
//                    tr(" associée au tour ") +
//                    trackIdentifier["lap"].toString() + tr(" de la course ") +
//                    trackIdentifier["race_num"].toString());
//        return;
//    }

//    QPlotCurve* curve = this->megasquirtDataPlot->addCurve(
//                tr("Course ") + trackIdentifier["race_num"].toString() +
//                tr(" tour ") + trackIdentifier["lap"].toString() +
//                " " + this->ui->megaSquirtComboBox->currentText(),
//                megasquirtCurvePoints);

//    this->setPlotCurveVisibile(curve, true);

    // =========================================================================
    // NOTE : ce qui suit est un test pour les courbes associées
//    QSqlQuery test("SELECT timestamp, speed FROM datarace WHERE ref_lap_race = ? and ref_lap_num = ? order by timestamp");
//    test.addBindValue(trackIdentifier["race"].toInt());
//    test.addBindValue(trackIdentifier["lap"].toInt());

//    if (!test.exec())
//    {
//        QMessageBox::warning(this, tr("Impossible de tracer la courbe"),
//                             tr("Erreur de récupération des données de vitesse ")
//                             + test.lastError().text());
//        return;
//    }

//    QVector<QPointF> points;
//    while(test.next())
//        points << QPointF(test.value(0).toFloat() / 1000,
//                          test.value(1).toFloat());

//    qDebug() << "Nombre de points dans la courbe vitesse = " << points.count();
//    QwtPointSeriesData* serie = new QwtPointSeriesData(points);
//    TrackPlotCurve* trackCurve = new TrackPlotCurve("vitesse", trackIdentifier, QPen("red"));
//    trackCurve->setData(serie);
//    trackCurve->attach(this->megasquirtDataPlot);
//    this->setPlotCurveVisibile(trackCurve, true);


//    // Courbe enfant
//    QSqlQuery test2("SELECT timestamp, distance FROM datarace WHERE ref_lap_race = ? and ref_lap_num = ? order by timestamp");
//    test2.addBindValue(trackIdentifier["race"].toInt());
//    test2.addBindValue(trackIdentifier["lap"].toInt());

//    if (!test2.exec())
//    {
//        QMessageBox::warning(this, tr("Impossible de tracer la courbe"),
//                             tr("Erreur de récupération des données de vitesse ")
//                             + test2.lastError().text());
//        return;
//    }

//    QVector<QPointF> points2;
//    while(test2.next())
//        points2 << QPointF(test2.value(0).toFloat() / 1000,
//                          test2.value(1).toFloat());

//    QwtPointSeriesData* serie2 = new QwtPointSeriesData(points2);
//    TrackPlotCurve* trackCurve2 = new TrackPlotCurve("enfant", trackIdentifier, QPen("red"));
//    trackCurve2->setData(serie2);
//    //trackCurve2->attach(this->megasquirtDataPlot);

//    //this->setPlotCurveVisibile(trackCurve2, true);

//    qDebug() << "MainWindow : avant de m'attacher ...";
//    trackCurve2->attachTo(trackCurve);
    // =========================================================================
}

void MainWindow::updateMenus(void)
{
    // Get the current plot
    Plot* plot = this->currentPlot();
    if (!plot) return;

    // Update menu edit actions
    this->ui->actionShowGrid->setChecked(plot->isGridVisible());
    this->ui->actionShowCrossLine->setChecked(plot->isCrossLineVisible());
    this->ui->actionShowLabelPosition->setChecked(plot->isLabelPositionVisible());
    this->ui->actionShowLabelPosition->setEnabled(!plot->isCrossLineVisible());
}

void MainWindow::on_actionClearCurve_triggered(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    // Si c'est une trackPlotCurve, on la supprime également des autres vues
    TrackPlotCurve* curve = (TrackPlotCurve*) this->curveAssociatedToLegendItem;
    if(curve)
        this->removeTrackFromAllView(curve->trackIdentifier());
    else
    {
        delete this->curveAssociatedToLegendItem;
        this->curveAssociatedToLegendItem = NULL;
    }

    // update the plot
    this->currentPlot()->replot();
}

void MainWindow::on_actionCentrerOnCurve_triggered(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    this->currentPlot()->zoom(this->curveAssociatedToLegendItem);
}

void MainWindow::on_actionChangeCurveColor_triggered(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    // Select a new color
    QColor newColor = QColorDialog::getColor(
                this->curveAssociatedToLegendItem->pen().color(), this,
                tr("Choisir une nouvelle couleur pour la courbe"));

    // If the user cancels the dialog, an invalid color is returned
    if (newColor.isValid())
        this->curveAssociatedToLegendItem->setColor(newColor);
}

void MainWindow::on_actionCreatePolynomialTrendline_triggered(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    bool ok(false);
    int degree = QInputDialog::getInt(
                this, tr("Courbe de tendance polynomiale"),
                tr("Ordre de complexité ?"), 2, 2, 100, 1, &ok);

    if (!ok) // User canceled
        return;

    // Récupération de la série des points de la courbe
    QwtPointSeriesData* curveSeriesData =
            (QwtPointSeriesData*)this->curveAssociatedToLegendItem->data();
    if (!curveSeriesData)
        return;

    // Récupération de la liste des points de la courbe
    QVector<QPointF> curvePoints(curveSeriesData->samples());

    // Calcul de tous les coefficients de l'équation
    QVector<double> coefficients(polynomialfit(curvePoints, degree));

    // Création de la liste des points de la courbe de tendance
    for(int i(0); i < curvePoints.count(); ++i)
    {
        // Calcul de la nouvelle valeur de y
        double y(0);
        for(int j(0); j < coefficients.count(); ++j)
            y += coefficients.at(j) * qPow(curvePoints.at(i).x(), j);

        curvePoints[i].setY(y);
    }

    // Création de la courbe
    QwtPointSeriesData* trendlineSeriesData =
            new QwtPointSeriesData(curvePoints);

    QPlotCurve* trendlineCurve = new QPlotCurve(
                this->curveAssociatedToLegendItem->title().text() +
                tr(" Poly(") + QString::number(degree) + ")",
                this->curveAssociatedToLegendItem->pen());
    trendlineCurve->setAxes(this->curveAssociatedToLegendItem->xAxis(),
                            this->curveAssociatedToLegendItem->yAxis());
    trendlineCurve->setData(trendlineSeriesData);
    trendlineCurve->attach(this->curveAssociatedToLegendItem->plot());
    this->setPlotCurveVisibile(trendlineCurve, true);
}

void MainWindow::on_actionRenameCurve_triggered(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    bool ok(false);
    QString newName = QInputDialog::getText(
                this, tr("Renommer une courbe"),
                tr("Nouveau nom pour la courbe ") +
                this->curveAssociatedToLegendItem->title().text() + " :",
                QLineEdit::Normal, QString(), &ok);

    if (!ok) // User canceled
        return;

    try
    {
        if (newName.isEmpty())
            throw QException(tr("Vous deve rentrer un nom pour la courbe"));

        // Check if curve name exists
        foreach (QwtPlotItem* item, this->currentPlot()->itemList())
            if (item->title().text() == newName)
                throw QException(tr("Une autre courbe porte déjà ce nom"));

        // Apply the new name
        this->curveAssociatedToLegendItem->setTitle(newName);
    }
    catch(QException const& ex)
    {
        QMessageBox::warning(this, tr("Impossible de renommer la courbe"),
                             ex.what());
    }
}

void MainWindow::on_actionChangeCurvePointsColor_triggered(void)
{
    TrackPlotCurve* curve = (TrackPlotCurve*) this->curveAssociatedToLegendItem;

    if(!curve)
        return;

    // Select a new color
    QColor newColor = QColorDialog::getColor(
                curve->pen().color(), this,
                tr("Choisir une nouvelle couleur pour la courbe"));

    // If the user cancels the dialog, an invalid color is returned
    if (!newColor.isValid())
        return;

    curve->setPointsColor(newColor);
    //curve->plot()->replot();
}

void MainWindow::on_actionClearChildrenCurves_triggered(void)
{
    TrackPlotCurve* curve = (TrackPlotCurve*) this->curveAssociatedToLegendItem;

    if(!curve)
        return;

    curve->clearChildren();
    curve->plot()->replot();
}

void MainWindow::setPlotCurveVisibile(QwtPlotItem* item, bool visible)
{
    if (item == NULL)
        return;

    item->setVisible(visible);
    QWidget* w = item->plot()->legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(visible);

    item->plot()->replot();
}

void MainWindow::showLegendContextMenu(const QwtPlotItem* item,
                                       const QPoint& pos)
{
    // Save the plot curve associated to the legend item
    this->curveAssociatedToLegendItem = (QPlotCurve*) item;
    if(!this->curveAssociatedToLegendItem)
        return;

    bool isTrackPlotCurve =
            item->rtti() == TrackPlotCurve::Rtti_TrackPlotCurveParent;
    this->ui->actionClearChildrenCurves->setVisible(isTrackPlotCurve);
    this->ui->actionChangeCurvePointsColor->setVisible(isTrackPlotCurve);


    // Display custom contextual menu
    this->legendContextMenu->exec(pos);
}

Plot* MainWindow::currentPlot(void) const
{
    switch (this->ui->plotsTabWidget->currentIndex())
    {
        case TAB_DISTANCE:
            return this->distancePlotFrame;
        case TAB_TIME:
            return this->timePlotFrame;
        case TAB_MEGASQUIRT:
            return this->megasquirtDataPlot;
        default:
            return NULL;
    }
}

void MainWindow::createPlotLegendContextMenu(void)
{
    // Legend actions
    this->legendContextMenu = new QMenu(this);

    // Add some "GUI created" actions
    QList<QAction*> actionList;
    actionList << this->ui->actionClearCurve
               << this->ui->actionCentrerOnCurve
               << this->ui->actionChangeCurveColor
               << this->ui->actionChangeCurvePointsColor
               << this->ui->actionClearChildrenCurves
               << this->ui->actionCreatePolynomialTrendline
               << this->ui->actionRenameCurve;

    this->legendContextMenu->addActions(actionList);
}

void MainWindow::on_actionShowGrid_triggered(bool visible)
{
    this->currentPlot()->setGridVisible(visible);
}

void MainWindow::on_actionShowCrossLine_triggered(bool visible)
{
    this->currentPlot()->setCrossLineVisible(visible);
    this->updateMenus(); // Because two menu actions must been (un)checked
}

void MainWindow::on_actionShowLabelPosition_triggered(bool visible)
{
    this->currentPlot()->setLabelPositionVisible(visible);
}

void MainWindow::on_actionIncreaseAccuracy_triggered(void)
{
    // Get the current plot
    Plot* plot = this->currentPlot();

    // Set the maximum number of major scale intervals for a specified axis
    plot->setAxisMaxMajor(QwtPlot::yLeft,
                          plot->axisMaxMajor(QwtPlot::yLeft) + 1);
    plot->setAxisMaxMajor(QwtPlot::yRight,
                          plot->axisMaxMajor(QwtPlot::yRight) + 1);
    plot->setAxisMaxMajor(QwtPlot::xBottom,
                          plot->axisMaxMajor(QwtPlot::xBottom) + 1);
}

void MainWindow::on_actionReduceAccuracy_triggered(void)
{
    // Get the current plot
    Plot* plot = this->currentPlot();

    // Set the maximum number of major scale intervals for a specified axis
    plot->setAxisMaxMajor(QwtPlot::yLeft,
                          plot->axisMaxMajor(QwtPlot::yLeft) - 1);
    plot->setAxisMaxMajor(QwtPlot::yRight,
                          plot->axisMaxMajor(QwtPlot::yRight) - 1);
    plot->setAxisMaxMajor(QwtPlot::xBottom,
                          plot->axisMaxMajor(QwtPlot::xBottom) - 1);
}

void MainWindow::on_actionExportToPDF_triggered(void)
{
    QString pdfFile = QFileDialog::getSaveFileName(
                this, tr("Sauvegarder le graphique"), QDir::homePath(),
                tr("Portable Document Format (*.pdf)"));

    if (pdfFile.isNull() || pdfFile.isEmpty()) // User canceled
        return;

    // Ask precision
    ExportConfigurationDialog exportConfig;
    if(exportConfig.exec() != QDialog::Accepted)
        return;

    // Add the current competition to the plot title just for export
    QString oldTitle = this->currentPlot()->title().text();

    this->currentPlot()->setTitle(this ->currentCompetition + " : " + oldTitle);

    QwtPlotRenderer renderer;
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground);
    renderer.renderDocument(
                this->currentPlot(), pdfFile,
                this->currentPlot()->size() / exportConfig.precision());

    // Restore title
    this->currentPlot()->setTitle(oldTitle);
}

void MainWindow::on_actionClearAllCurves_triggered(void)
{
    Plot* plot = this->currentPlot();

    if(plot)
        plot->clearcurves();
}

void MainWindow::on_actionGlobalPlotCurvesView_triggered(void)
{
    Plot* plot = this->currentPlot();

    if(plot)
        plot->globalZoom();
}

void MainWindow::on_actionClearSecondaryCurves_triggered(void)
{
    AdvancedPlot* plot = (AdvancedPlot*) this->currentPlot();

    plot->clearSecondaryCurves();

}

void MainWindow::on_actionChangeFileNames_triggered(void)
{
    FileParameterDialog dial(this);
    dial.exec();
}
