#include "ColorPicker.hpp"

ColorPicker::ColorPicker(int num) :
    _inc(0), _nbColors(num)
{
    if (num == 0)
        num = 1;
    this->_inc = 360 / (num % 360);
}

QColor ColorPicker::color(int index, int alpha) const
{
    return QColor::fromHsv((this->_inc * index) % 360, 230, 230, alpha);
}

QColor ColorPicker::light(int index, int alpha) const
{
    return QColor::fromHsv((this->_inc * index) % 360, 255, 255, alpha);
}

QColor ColorPicker::dark(int index, int alpha) const
{
    return QColor::fromHsv((this->_inc * index) % 360, 200, 200, alpha);
}

QColor ColorPicker::colorRandom(void) const
{
    // qrand() % number --> returns a radom number a, 0 <= a < number
    return this->color(qrand() % this->_nbColors);
}

QColor ColorPicker::lightRandom(void) const
{
    // qrand() % number --> returns a radom number a, 0 <= a < number
    return this->light(qrand() % this->_nbColors);
}

QColor ColorPicker::darkRandom(void) const
{
    // qrand() % number --> returns a radom number a, 0 <= a < number
    return this->dark(qrand() % this->_nbColors);
}

QColor ColorPicker::random(void) const
{
    switch (qrand() % 3)
    {
        case 0:
            return this->colorRandom();
        case 1:
            return this->lightRandom();
        default:
            return this->darkRandom();
    }
}
