#include "AdvancedPlot.hpp"

AdvancedPlot::AdvancedPlot(QString const& title, int nbColor, QWidget* parent) :
    AdvancedPlot(QwtText(title), nbColor, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

AdvancedPlot::AdvancedPlot(QwtText const& title, int nbColor, QWidget* parent) :
    Plot(title, parent), colorPicker(nbColor)
{
    // Désactive le rubberBand du zoomer pour celui du picker
    this->_xBottomYLeftZoomer->setRubberBand(QwtPicker::NoRubberBand);
    this->_xBottomYLeftZoomer->setTrackerMode(QwtPicker::AlwaysOff);

//    // Picker with click point machine to provide point selection
//    QwtPlotPicker* clickPicker = new QwtPlotPicker(this->canvas());
//    clickPicker->setStateMachine(new QwtPickerClickPointMachine);
//    clickPicker->setMousePattern(0,Qt::LeftButton,Qt::SHIFT);
//    connect(clickPicker, SIGNAL(appended(QPointF)),
//            this, SLOT(pointSelected(QPointF)));

    // Picker with drag rect machine to provide multiple points selection
    QwtPlotPicker* rectPicker = new QwtPlotPicker(this->canvas());
    rectPicker->setRubberBand(QwtPicker::RectRubberBand);
    rectPicker->setStateMachine(new QwtPickerDragRectMachine);
    connect(rectPicker, SIGNAL(selected(QRectF)),
            this, SLOT(pointsSelected(QRectF)));
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

void AdvancedPlot::pointsSelected(const QRectF &selectedRect)
{
    if(selectedRect.left() == selectedRect.right())
        qDebug() << "On a sélectionné qu'un seul point ...";
    else
        qDebug() << "On a sélectionné des points ....";

    foreach (QwtPlotItem* item, this->itemList(QwtPlotItem::Rtti_PlotCurve))
    {
        TrackPlotCurve* curve = (TrackPlotCurve*) item;

        // la seletion ne peut se faire que sur une courbe parente
        if(curve == NULL || curve->parent() != NULL)
            continue;

        // La selection ne peut se faire que sur les courbes visibles
        if(!curve->isVisible())
            continue;

        if (!curve->boundingRect().intersects(selectedRect))
            continue;

        // Recherche des points les plus proches des extremités du rectangle
        qDebug() << "coupe la ligne : " << curve->title().text();

        curve->closestPointOfX(selectedRect.left());
    }
}

void AdvancedPlot::pointSelected(const QPointF &point)
{
    double minDist = 10e10;
    int index(-1);
    TrackPlotCurve* curve(NULL);

    foreach (QwtPlotItem* item, this->itemList(QwtPlotItem::Rtti_PlotCurve))
    {
        TrackPlotCurve* tmpCurve = (TrackPlotCurve*) item;

        // la seletion ne peut se faire que sur une courbe parente
        if(tmpCurve == NULL || tmpCurve->parent() != NULL || !tmpCurve->isVisible())
        {
            qDebug() << "Point Selected : pas une bonne courbe";
            continue;
        }

        double tmpDist;
        int tmpIndex = tmpCurve->closestPoint(point.toPoint(), &tmpDist);
        if (tmpDist < minDist && tmpIndex > -1)
        {
            curve = tmpCurve;
            minDist = tmpDist;
            index = tmpIndex;
        }
    }

    if(curve != NULL)
    qDebug() << "Point le plus proche = " << curve->sample(index)
             << "courbe = " << curve->title().text();
}

void AdvancedPlot::selectedPoints(const QVector<QPointF> &points)
{
    qDebug() << "Nombre de points sélectionnées = " << points.count();
}
