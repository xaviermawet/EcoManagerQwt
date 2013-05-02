/* TODO
 * ---------------
 *
 *  Bouger cette classe et la mettre dans le dossier Utils
 */

#ifndef __COLORPICKER_HPP__
#define __COLORPICKER_HPP__

#include <QColor>

class ColorPicker
{
    public:

        ColorPicker(int num);

        // Getters
        QColor color(int ind, int alpha = 255) const;
        QColor light(int ind, int alpha = 255) const;
        QColor dark (int ind, int alpha = 255) const;

        QColor colorRandom(void) const;
        QColor lightRandom(void) const;
        QColor darkRandom (void) const;

        QColor random(void) const;

    protected:

        int _inc;
        int _nbColors;
};

#endif /* __COLORPICKER_HPP__ */
