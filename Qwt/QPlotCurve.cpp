#include "QPlotCurve.hpp"

QPlotCurve::QPlotCurve(const QString &title, const QPen &pen) :
    QPlotCurve(QwtText(title), pen)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

QPlotCurve::QPlotCurve(const QwtText &title, const QPen &pen) :
    QwtPlotCurve(title)
{
    this->setPen(pen);
    this->setRenderHint(QwtPlotItem::RenderAntialiased);
    this->setItemAttribute(QwtPlotItem::Legend); // true default value
    //this->setLegendAttribute(LegendShowLine);
}

int QPlotCurve::rtti(void) const
{
    return Rtti_CustomPlotCurve;
}

void QPlotCurve::setColor(const QColor &color)
{
    if (!color.isValid())
        return;

    QPen pen = this->pen();
    pen.setColor(color);

    this->setPen(pen);
}
