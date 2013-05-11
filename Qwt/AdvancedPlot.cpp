#include "AdvancedPlot.hpp"

AdvancedPlot::AdvancedPlot(QString const& title, int nbColor, QWidget* parent) :
    AdvancedPlot(QwtText(title), nbColor, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

AdvancedPlot::AdvancedPlot(QwtText const& title, int nbColor, QWidget* parent) :
    Plot(title, parent), colorPicker(nbColor)
{
    /* je ne pouvais pas utiliser le Zoomer comme picker car il ne génère pas
     * d'évènement lors d'un simple clic, donc dans tous les cas, je devais
     * ajouter un nouveau picker */

    // Picker with drag rect machine to provide multiple points selection
    QwtPlotPicker* rectPicker = new QwtPlotPicker(this->canvas());
    rectPicker->setStateMachine(new QwtPickerDragRectMachine);
    connect(rectPicker, SIGNAL(selected(QRectF)),
            this, SLOT(pointsSelected(QRectF)));
}

AdvancedPlot::~AdvancedPlot(void)
{
    qDebug() << "AdvancedPlot (" << this->objectName() << ") Début destructeur";

    // Supprimer les courbes qui on des enfants
    foreach (QwtPlotItem* item, this->itemList(QwtPlotItem::Rtti_PlotCurve))
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

void AdvancedPlot::pointsSelected(const QRectF &selectedRect)
{
    if (selectedRect.left() == selectedRect.right() &&
        selectedRect.top() == selectedRect.bottom())
    {
        if (this->isCrossLineVisible())
        {
            qDebug() << "On veut un point de toutes les courbes...";
            this->pointSelected(selectedRect.topLeft()); // Top left au hasard, les 4 extrémités sont sur le meme point
        }
        else
            qDebug() << "Sélection d'un seul point ...";
        return;
    }


    qDebug() << "Rectangle de sélection : Left = " << selectedRect.left() << " right = " << selectedRect.right();

    foreach (QwtPlotItem* item, this->itemList(TrackPlotCurve::Rtti_TrackPlotCurve))
    {
        TrackPlotCurve* curve = (TrackPlotCurve*) item;

        // la seletion ne peut se faire que sur une courbe parente et visible
        if(curve == NULL || curve->parent() != NULL || !curve->isVisible())
            continue;

        if (!curve->boundingRect().intersects(selectedRect))
            continue;

        // Récupérer la liste des points de la courbe
        QwtSeriesData<QPointF>* curvePoints = curve->data();

        QPolygonF pointsSelected;

        qDebug() << "Nombre de points dans la courbe " << curve->title().text()
                 << " = " << curvePoints->size();

        unsigned int i;
        for(i = 0;i < curvePoints->size()
                   && curvePoints->sample(i).x() < selectedRect.left(); ++i);

        for(i; i < curvePoints->size()
               && curvePoints->sample(i).x() < selectedRect.right(); ++i)
            if (selectedRect.contains(curvePoints->sample(i)))
                pointsSelected << curvePoints->sample(i);

        if (!pointsSelected.isEmpty())
        {
            qDebug() << "Premier = " << pointsSelected.first() << " dernier = " << pointsSelected.last();

            emit this->intervalSelected(pointsSelected.first().x(),
                                        pointsSelected.last().x(),
                                        curve->trackIdentifier());
        }
    }
}

void AdvancedPlot::pointSelected(const QPointF &point)
{
    qDebug() << "Point sélectionné = " << point;

    foreach (QwtPlotItem* item, this->itemList(TrackPlotCurve::Rtti_TrackPlotCurve))
    {
        TrackPlotCurve* curve = (TrackPlotCurve*) item;

        // la seletion ne peut se faire que sur une courbe parente
        if(curve == NULL || curve->parent() != NULL || !curve->isVisible())
            continue;

        qDebug() << "courbe = " << curve->title().text()
                 << " Point le plus proche = " << curve->closestPointFOfX(point.x(), NULL);
    }
}
