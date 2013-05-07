#include "TrackPlotCurve.hpp"

TrackPlotCurve::TrackPlotCurve(
        const QString &title, const TrackIdentifier &trackId,
        const QPen &pen, TrackPlotCurve *parent) :
    TrackPlotCurve(QwtText(title), trackId, pen, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

TrackPlotCurve::TrackPlotCurve(
        QwtText const& title, TrackIdentifier const& trackId,
        QPen const& pen, TrackPlotCurve* parent) :
    QPlotCurve(title, pen), _trackIdentifier(trackId), parentCurve(NULL)
{
    this->attachTo(parent);
}

TrackPlotCurve::~TrackPlotCurve(void)
{
    foreach (TrackPlotCurve* childCurve, this->children)
        if(childCurve->parent() == NULL)
            delete childCurve;
}

TrackIdentifier TrackPlotCurve::trackIdentifier(void) const
{
    return this->_trackIdentifier;
}

TrackPlotCurve const* TrackPlotCurve::parent(void) const
{
    return this->parentCurve;
}

void TrackPlotCurve::setVisible(bool visible)
{
    qDebug() << "On change la visibilité ...";

    QPlotCurve::setVisible(visible);

    // Hide or show child curves
    foreach (TrackPlotCurve* childCurve, this->children)
        childCurve->setVisible(visible);
}

void TrackPlotCurve::attachChild(TrackPlotCurve* child)
{
    if (child == NULL)
        return;

    // Detach the child from previous parent
    child->detachFromParentCurve();

    this->children.append(child);
    child->parentCurve = this;

    // Change legend item visibility to false (only the parent is visible)
    child->setItemAttribute(Legend, false);

    // Attach the child in the same plot as its new parent
    child->attach(this->plot());
}

void TrackPlotCurve::attachTo(TrackPlotCurve* parent)
{
    if(parent == NULL)
    {
        this->detachFromParentCurve();
        return;
    }

    parent->attachChild(this);
}

void TrackPlotCurve::detachFromParentCurve(void)
{
    if (parentCurve == NULL)
        return;

    this->parentCurve->removeChild(this);
    this->parentCurve = NULL;

    // Change legend item visibility
    this->setItemAttribute(Legend, true);
}

void TrackPlotCurve::attach(QwtPlot *plot)
{
    qDebug() << "On attache la courbe courante et tous ses enfants ...";

    QPlotCurve::attach(plot);

    foreach (TrackPlotCurve* childCurve, this->children)
        childCurve->attach(plot);
}

void TrackPlotCurve::detach(void)
{
    qDebug() << "On détache la courbe courante et tous ses enfants ...";

    QPlotCurve::detach();

    foreach (TrackPlotCurve* childCurve, this->children)
        childCurve->detach();
}

QRectF TrackPlotCurve::boundingRect(void) const
{
    QRectF rect = QwtPlotCurve::boundingRect();

    foreach (TrackPlotCurve* child, this->children) {
        rect = rect.united(child->boundingRect());
    }

    return rect;
}

void TrackPlotCurve::removeChild(TrackPlotCurve * const &child)
{
    this->children.removeOne(child);
}
