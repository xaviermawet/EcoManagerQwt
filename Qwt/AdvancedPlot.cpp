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

    // Picker with click point machine to provide point selection
    QwtPlotPicker* clickPicker = new QwtPlotPicker(this->canvas());
    clickPicker->setStateMachine(new QwtPickerClickPointMachine);
    clickPicker->setMousePattern(0,Qt::LeftButton,Qt::SHIFT);
    connect(clickPicker, SIGNAL(appended(QPointF)),
            this, SLOT(pointSelected(QPointF)));

    // Picker with drag rect machine to provide multiple points selection
    QwtPlotPicker* rectPicker = new QwtPlotPicker(
                this->xBottom, this->yLeft, QwtPicker::RectRubberBand,
                QwtPicker::AlwaysOff, this->canvas());
    QwtPickerDragRectMachine* test = new QwtPickerDragRectMachine();
    test->setState(QwtPickerMachine::RectSelection);
    rectPicker->setStateMachine(test);
    connect(rectPicker, SIGNAL(selected(QRectF)),
            this, SLOT(pointsSelected(QRectF)));

    connect(clickPicker, SIGNAL(selected(QVector<QPointF>)),
            this, SLOT(selectedPoints(QVector<QPointF>)));
    connect(rectPicker, SIGNAL(selected(QVector<QPointF>)),
            this, SLOT(selectedPoints(QVector<QPointF>)));
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
    foreach (QwtPlotItem* item, this->itemList())
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
        qDebug() << "couple la ligne : " << curve->title().text();

        curve->closestPointOfX(selectedRect.left());
    }
}

void AdvancedPlot::pointSelected(const QPointF &point)
{
    qDebug() << "un point a été sélectionné : " << point;
}

void AdvancedPlot::selectedPoints(const QVector<QPointF> &points)
{
    qDebug() << "Nombre de points sélectionnées = " << points.count();
}
