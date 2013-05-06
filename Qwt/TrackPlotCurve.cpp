#include "TrackPlotCurve.hpp"

TrackPlotCurve::TrackPlotCurve(
        const QString &title, const TrackIdentifier &trackId,
        const QPen &pen, TrackPlotCurve *parent) :
    TrackPlotCurve(title, trackId, pen, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

TrackPlotCurve::TrackPlotCurve(
        const QwtText &title, const TrackIdentifier &trackId,
        const QPen &pen, TrackPlotCurve *parent) :
    QPlotCurve(title, pen), _trackIdentifier(trackId), parentCurve(parent)
{
}

TrackPlotCurve::~TrackPlotCurve(void)
{
    foreach (TrackPlotCurve* childCurve, this->children)
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
    QPlotCurve::setVisible(visible);

    // Hide or show child curves
    foreach (TrackPlotCurve* childCurve, this->children)
        childCurve->setVisible(visible);
}

void TrackPlotCurve::attach(TrackPlotCurve* child)
{
    // Detach the child from previous parent
    child->detachFromParentCurve();

    this->children.append(child);
    child->parentCurve = this;
}

void TrackPlotCurve::attachTo(TrackPlotCurve* parent)
{
    parent->attach(this);
}

void TrackPlotCurve::detachFromParentCurve(void)
{
    if (parentCurve == NULL)
        return;

    this->parentCurve->removeChild(this);
    this->parentCurve = NULL;
}

void TrackPlotCurve::removeChild(TrackPlotCurve * const &child)
{
    this->children.removeOne(child);
}
