#-------------------------------------------------
#
# Project created by QtCreator 2013-02-24T19:15:10
#
#-------------------------------------------------

QT       += core gui sql
CONFIG   += qwt
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EcoManager2013
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    CompetitionEntryDialog.cpp \
    Map/SampleLapViewer.cpp \
    Map/MapView.cpp \
    Common/IndexedPosition.cpp \
    Common/CoordinateItem.cpp \
    Map/SectorItem.cpp \
    Map/CurvePathBuilder.cpp \
    Map/AnimateSectorItem.cpp \
    Map/TrackItem.cpp \
    Map/TickItem.cpp \
    Map/PathBuilder.cpp \
    Map/MapScene.cpp \
    DBModule/GeoCoordinate.cpp \
    Map/MapFrame.cpp \
    DBModule/DataPoint.cpp \
    DBModule/ExportModule.cpp \
    DBModule/Zone.cpp \
    DBModule/Race.cpp \
    DBModule/LapDetector.cpp \
    ExtensibleEllipseItem.cpp \
    RaceViewer.cpp \
    DBModule/ImportModule.cpp \
    Common/ColorizerProxyModel.cpp \
    Common/TreeItem.cpp \
    Common/HierarchicalProxyModel.cpp \
    Common/GroupProxyModel.cpp \
    Common/GroupingTreeModel.cpp \
    CompetitionProxyModel.cpp \
    Plot/Scale.cpp \
    Plot/HorizontalScale.cpp \
    Plot/VerticalScale.cpp \
    Plot/PlotView.cpp \
    Plot/PlotCurve.cpp \
    Common/TreeLapInformationModel.cpp \
    Common/TreeNode.cpp \
    LapInformationProxyModel.cpp \
    Plot/PlotScene.cpp \
    Plot/PlotFrame.cpp \
    Common/LapInformationTreeModel.cpp \
    Common/TreeElement.cpp \
    LapDataCompartor.cpp \
    Plot/PlotPrintDialog.cpp \
    Utils/QException.cpp \
    Utils/DataBaseManager.cpp \
    Utils/QCSVParser.cpp \
    DBModule/DataBaseImportModule.cpp \
    Megasquirt/MSDataConverter.cpp \
    Megasquirt/MSManager.cpp \
    Qwt/Plot.cpp \
    Qwt/AbstractDoubleAxisPlot.cpp \
    Qwt/DoubleYAxisPlot.cpp \
    Qwt/DoubleXAxisPlot.cpp \
    Qwt/PlotMagnifier.cpp \
    Qwt/Zoomer.cpp \
    Qwt/QPlotCurve.cpp \
    FileParameterDialog.cpp \
    Qwt/AdvancedPlot.cpp \
    Utils/ColorPicker.cpp \
    Qwt/TrackPlotCurve.cpp \
    ExportConfigurationDialog.cpp

HEADERS  += MainWindow.hpp \
    CompetitionEntryDialog.hpp \
    Map/SampleLapViewer.hpp \
    Map/MapView.hpp \
    Common/IndexedPosition.hpp \
    Common/CoordinateItem.hpp \
    Map/SectorItem.hpp \
    Map/CurvePathBuilder.hpp \
    Map/AnimateSectorItem.hpp \
    Map/TrackItem.hpp \
    Map/TickItem.hpp \
    Map/PathBuilder.hpp \
    Map/MapScene.hpp \
    DBModule/GeoCoordinate.hpp \
    Map/MapFrame.hpp \
    DBModule/DataPoint.hpp \
    DBModule/ExportModule.hpp \
    DBModule/Zone.hpp \
    DBModule/Race.hpp \
    DBModule/LapDetector.hpp \
    ExtensibleEllipseItem.hpp \
    RaceViewer.hpp \
    DBModule/ImportModule.hpp \
    Common/ColorizerProxyModel.hpp \
    Common/TreeItem.hpp \
    Common/HierarchicalProxyModel.hpp \
    Common/GroupProxyModel.hpp \
    Common/GroupingTreeModel.hpp \
    CompetitionProxyModel.hpp \
    Plot/Scale.hpp \
    Plot/HorizontalScale.hpp \
    Plot/VerticalScale.hpp \
    Plot/PlotView.hpp \
    Plot/PlotCurve.hpp \
    Common/TreeLapInformationModel.hpp \
    Common/TreeNode.hpp \
    LapInformationProxyModel.hpp \
    Plot/PlotScene.hpp \
    Plot/PlotFrame.hpp \
    Common/LapInformationTreeModel.hpp \
    Common/TreeElement.hpp \
    LapDataCompartor.hpp \
    Plot/PlotPrintDialog.hpp \
    Utils/QException.hpp \
    Utils/DataBaseManager.hpp \
    Utils/QCSVParser.hpp \
    DBModule/DataBaseImportModule.hpp \
    Megasquirt/MSDataConverter.hpp \
    Megasquirt/MSManager.hpp \
    Qwt/Plot.hpp \
    Qwt/AbstractDoubleAxisPlot.hpp \
    Qwt/DoubleYAxisPlot.hpp \
    Qwt/DoubleXAxisPlot.hpp \
    Qwt/PlotMagnifier.hpp \
    Qwt/Zoomer.hpp \
    Qwt/QPlotCurve.hpp \
    FileParameterDialog.hpp \
    Qwt/AdvancedPlot.hpp \
    Utils/ColorPicker.hpp \
    Qwt/TrackPlotCurve.hpp \
    ExportConfigurationDialog.hpp

FORMS    += MainWindow.ui \
    CompetitionEntryDialog.ui \
    Map/SampleLapViewer.ui \
    Map/MapFrame.ui \
    Plot/PlotFrame.ui \
    LapDataCompartor.ui \
    Plot/PlotPrintDialog.ui \
    FileParameterDialog.ui \
    ExportConfigurationDialog.ui

RESOURCES += \
    Resources.qrc
