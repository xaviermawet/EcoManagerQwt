#include "AdvancedPlot.hpp"

AdvancedPlot::AdvancedPlot(QString const& title, int nbColor, QWidget* parent) :
    AdvancedPlot(QwtText(title), nbColor, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

AdvancedPlot::AdvancedPlot(QwtText const& title, int nbColor, QWidget* parent) :
    Plot(title, parent), colorPicker(nbColor)
{
    // Gestion des évènements du zoomer (selection de zone etc ...)
}

AdvancedPlot::~AdvancedPlot(void)
{
    qDebug() << "AdvancedPlot (" << this->objectName() << ") Début destructeur";

    // Supprimer les courbes qui on des enfants
    foreach (QwtPlotItem* item, this->itemList())
    {
        TrackPlotCurve* curve = (TrackPlotCurve*) item;

        if(curve == NULL)
            continue;

        if (curve->parent() == NULL)
            delete curve;
    }

    qDebug() << "AdvancedPlot (" << this->objectName() << ") fin destructeur";
}

QPlotCurve* AdvancedPlot::addCurve(const QString& title,
                                  const QVector<QPointF>& points)
{
    return Plot::addCurve(
               title, points,this->colorPicker.light(this->itemList().count()));
}
