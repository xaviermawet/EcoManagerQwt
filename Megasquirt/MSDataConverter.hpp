#ifndef __MEGASQUIRTDATACONVERTER_HPP__
#define __MEGASQUIRTDATACONVERTER_HPP__

#if QT_VERSION >= 0x050000 //  0xMMNNPP (MM = major, NN = minor, PP = patch)
    #include <QtWidgets>
#else
    #include <QtGui>
#endif

class MSDataConverter
{
    public:

        typedef enum
        {
            U08 = 1,
            U16 = 2,
            U32 = 3,
            S08 = 4,
            S16 = 5,
            S32 = 6
        } MSDataSize;

        MSDataConverter(const QString& field, MSDataSize type,
                                int offset, const QString& unit,
                                double scale, double translate = 0);

        // Getter
        QString field(void) const;
        MSDataSize type(void) const;
        int offset(void) const;
        QString unit(void) const;
        double scale(void) const;
        double translate(void) const;

        // Setter
        void setField(const QString& field);
        void setType(MSDataSize type);
        void setOffset(int offset);
        void setUnit(const QString& unit);
        void setScale(double scale);
        void setTranslate(double translate);

    protected:

        QString     _field;
        MSDataSize  _type;
        int         _offset;
        QString     _unit;
        double      _scale;
        double      _translate;
};

#endif /* __MEGASQUIRTDATACONVERTER_HPP__ */
