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

    this->setTitle(title.text() + QObject::tr(" (course ") +
                   trackId["race_num"].toString() + QObject::tr(" tour ") +
            trackId["lap"].toString() + ")");

    // Mise en évidence des points de la courbe si c'est une courbe parent
    if (this->parentCurve == NULL)
        this->setSymbol(new QwtSymbol(
                            QwtSymbol::Ellipse, Qt::gray, pen, QSize(2, 2)));
}

TrackPlotCurve::~TrackPlotCurve(void)
{
    qDebug() << "TrackPlotCurve, suppression de : " << this->title().text();

    this->detach();

    this->clearChildren();
}

int TrackPlotCurve::rtti(void) const
{
    // If it's a child
    if(this->parentCurve)
        return Rtti_TrackPlotCurveChild;

    return Rtti_TrackPlotCurveParent;
}

void TrackPlotCurve::setColor(const QColor &color)
{
    // change la couleur de la courbe
    QPlotCurve::setColor(color);

    foreach (TrackPlotCurve* curve, this->children)
        curve->setColor(color);

    // Change la couleur des points
    const QwtSymbol* symbol = this->symbol();
    if (!symbol)
        return;

    if (this->parentCurve == NULL)
        this->setSymbol(new QwtSymbol(
                            QwtSymbol::Ellipse, Qt::gray, QPen(color), QSize(2, 2)));
}

void TrackPlotCurve::setPointsColor(QColor const& color)
{
    const QwtSymbol* symbol = this->symbol();

    if(!symbol || !color.isValid())
        return;

    qDebug() << "Changement de couleur des points ...";
//    QwtSymbol* newSymbol = new QwtSymbol(*symbol);
//    newSymbol->setColor(color);
//    this->setSymbol(newSymbol);

    this->setSymbol(new QwtSymbol(
                        QwtSymbol::Ellipse, Qt::gray, QPen(color), QSize(2, 2)));
}

TrackIdentifier TrackPlotCurve::trackIdentifier(void) const
{
    return this->_trackIdentifier;
}

TrackPlotCurve const* TrackPlotCurve::parent(void) const
{
    return this->parentCurve;
}

QRectF TrackPlotCurve::boundingRect(void) const
{
    QRectF rect = QwtPlotCurve::boundingRect();

    foreach (TrackPlotCurve* child, this->children) {
        rect = rect.united(child->boundingRect());
    }

    return rect;
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

    child->_trackIdentifier = this->_trackIdentifier;

    child->setSymbol(NULL);
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
}

bool TrackPlotCurve::removeChild(TrackPlotCurve * const &child)
{
    if(!this->children.contains(child))
        return false;

    if(!this->children.removeOne(child))
        return false;

    child->parentCurve = NULL;

    // Change legend item visibility
    child->setItemAttribute(Legend, true);

    return true;
}

void TrackPlotCurve::clearChildren(void)
{
    while(!this->children.isEmpty())
        delete this->children.takeFirst();
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

QPointF TrackPlotCurve::closestPointF(QPointF const& pos, double* dist) const
{
    // Récupérer la liste des points
    const QwtSeriesData<QPointF>* curvePoints = this->data();

    /* On suppose que tous les points sont triés par ordre croissant suivant
     * leur valeur en abscisse
     * --> Effectuer une recherche dichotomique sur la valeur en x */
    int indiceMin(0);
    int indiceMax(curvePoints->size() - 1);

    while (indiceMin <= indiceMax)
    {
        unsigned int mid = (indiceMin + indiceMax) / 2;
        if (curvePoints->sample(mid).x() < pos.x())
            indiceMin = mid + 1;
        else
            indiceMax = mid - 1;
    }

    // Si c'est le premier point
    if (indiceMin == 0)
    {
        QPointF pointF = curvePoints->sample(indiceMin);
        if (dist)
            *dist = qSqrt(qPow(pointF.x() - pos.x(), 2) + qPow(pointF.y() - pos.y(), 2));
        return pointF;
    }

    // Si c'est le dernier point
    if (indiceMin >= (int)curvePoints->size())
    {
        QPointF pointF = curvePoints->sample(indiceMax);
        if (dist)
            *dist = qSqrt(qPow(pointF.x() - pos.x(), 2) + qPow(pointF.y() - pos.y(), 2));
        return pointF;
    }

    /* Deux points possibles --> choix du plus proche on fonction de leur
     * distance par rapport au point initial (pointF du clic */
    QPointF pointF1 = curvePoints->sample(indiceMin); // le point après la pos
    QPointF pointF2 = curvePoints->sample(indiceMax); // Le point avant la pos

    double distF1 = qSqrt(qPow(pointF1.x() - pos.x(), 2) + qPow(pointF1.y() - pos.y(), 2));
    double distF2 = qSqrt(qPow(pointF2.x() - pos.x(), 2) + qPow(pointF2.y() - pos.y(), 2));

    if (distF1 < distF2)
    {
        if (dist)
            *dist = distF1;
        return pointF1;
    }

    if (dist)
        *dist = distF2;
    return pointF2;
}

QPointF TrackPlotCurve::closestPointFOfX(const qreal &posX, double *dist) const
{
    // Récupérer la liste des points
    const QwtSeriesData<QPointF>* curvePoints = this->data();

    /* On suppose que tous les points sont triés par ordre croissant suivant
     * leur valeur en abscisse
     * --> Effectuer une recherche dichotomique sur la valeur en x */
    int indiceMin(0);
    int indiceMax(curvePoints->size() - 1);

    while (indiceMin <= indiceMax)
    {
        unsigned int mid = (indiceMin + indiceMax) / 2;

        qDebug() << "mid = " << mid << " min = " << indiceMin << " max = " << indiceMax;

        if (curvePoints->sample(mid).x() < posX)
            indiceMin = mid + 1;
        else
            indiceMax = mid - 1;
    }

    // Si c'est le premier point
    if (indiceMin == 0)
    {
        qDebug() << "C'est le tout premier point ...";
        QPointF pointF = curvePoints->sample(indiceMin);
        if (dist)
            *dist = qAbs(pointF.x() - posX);
        return pointF;
    }

    // Si c'est le dernier point
    if (indiceMin >= (int)curvePoints->size())
    {
        qDebug() << "C'est le tout dernier point ...";
        QPointF pointF = curvePoints->sample(indiceMax);
        if (dist)
            *dist = qAbs(pointF.x() - posX);
        return pointF;
    }

    /* Deux points possibles --> choix du plus proche on fonction
     * des différences de position en X */
    qDebug() << "indiceMin = " << indiceMin << " indiceMax = " << indiceMax;

    QPointF pointF1 = curvePoints->sample(indiceMin); // le point après la pos
    QPointF pointF2 = curvePoints->sample(indiceMax); // Le point avant la pos

    double distF1 = qAbs(pointF1.x() - posX);
    double distF2 = qAbs(pointF2.x() - posX);

    qDebug() << "dist point avant = " << distF2;
    qDebug() << "dist point après = " << distF1;

    if (distF1 < distF2)
    {
        if (dist != NULL)
            *dist = distF1;
        return pointF1;
    }

    if (dist!= NULL)
        *dist = distF2;
    return pointF2;
}
