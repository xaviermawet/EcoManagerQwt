#ifndef __QEXCEPTION_HPP__
#define __QEXCEPTION_HPP__

#include <QString>

class QException
{
    public:

        explicit QException(QString const& message, int code = -1);
        QException(QException const& copy);
        virtual QException& operator=(QException const& copy);
        virtual ~QException(void) throw();

        virtual QString what(void) const throw();
        virtual int     code(void) const throw();

    protected:

        QString _message;
        int     _code;
};

#endif /* __QEXCEPTION_HPP__ */
