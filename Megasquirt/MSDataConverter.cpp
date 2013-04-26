#include "MSDataConverter.hpp"

MSDataConverter::MSDataConverter(
        const QString &field, MSDataSize type, int offset,
        const QString &unit, double scale, double translate)
{
    this->setField(field);
    this->setType(type);
    this->setOffset(offset);
    this->setUnit(unit);
    this->setScale(scale);
    this->setTranslate(translate);
}

QString MSDataConverter::field(void) const
{
    return this->_field;
}

MSDataConverter::MSDataSize MSDataConverter::type(void) const
{
    return this->_type;
}

int MSDataConverter::offset(void) const
{
    return this->_offset;
}

QString MSDataConverter::unit(void) const
{
    return this->_unit;
}

double MSDataConverter::scale(void) const
{
    return this->_scale;
}

double MSDataConverter::translate(void) const
{
    return this->_translate;
}

void MSDataConverter::setField(const QString &field)
{
    this->_field = field;
}

void MSDataConverter::setType(MSDataSize type)
{
    this->_type = type;
}

void MSDataConverter::setOffset(int offset)
{
    this->_offset = offset;
}

void MSDataConverter::setUnit(const QString &unit)
{
    this->_unit = unit;
}

void MSDataConverter::setScale(double scale)
{
    this->_scale = scale;
}

void MSDataConverter::setTranslate(double translate)
{
    this->_translate = translate;
}
