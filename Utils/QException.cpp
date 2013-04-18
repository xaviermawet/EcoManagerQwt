#include "QException.hpp"

QException::QException(const QString &message, int code) :
    _message(message), _code(code)
{
}

QException::QException(const QException &copy) :
    QException(copy._message, copy._code)
{
    // No pointer to manage
}

QException& QException::operator=(QException const& copy)
{
    if (this != &copy)
        this->_message = copy._message;

    return *this;
}

QException::~QException(void) throw()
{
    // No pointer to manage
}

QString QException::what(void) const throw()
{
    return this->_message;
}

int QException::code(void) const throw()
{
    return this->_code;
}
