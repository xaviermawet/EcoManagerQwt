#ifndef __ZOOMER_HPP__
#define __ZOOMER_HPP__

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>

/*!
 * \brief Gère un pile de zooms pour un Plot ainsi que l'adaptation de l'échelle
 * d'un axe x et un axe y
 *
 * un Zoomer permet de délimiter une zone rectangulaire dans un Plot, de zoomer
 * sur cette zone et de la translater en coordonnées afin d'adapter les axes.
 * Chaque rectangle ainsi délimité est mis en haut d'une pile.
 *
 * Un Zoomer ne peut gérer qu'un axe x et un axe y. Mais rien ne vous empêche
 * d'attacher un second Zoomer au même Plot pour gérer les autres axes
 */
class Zoomer: public QwtPlotZoomer
{
    Q_OBJECT

    public:

        /*!
         * \brief Constructeur
         * \param canvas La toile du Plot
         * \param doReplot Appeler la méthode replot du Plot avant d'initialiser
         * le Zoomer avec ses échelles.
         */
        explicit Zoomer(QwtPlotCanvas* canvas, bool doReplot = true);

        /*!
         * \brief Constructeur
         * \param xAxis L'axe x du Zoomer
         * \param yAxis L'axe y du Zoomer
         * \param canvas La toile du Plot
         * \param doReplot Appeler la méthode replot du Plot avant d'initialiser
         * le Zoomer avec ses échelles.
         */
        explicit Zoomer(int xAxis, int yAxis, QwtPlotCanvas* canvas,
                        bool doReplot = true);

        /*!
         * \brief Translate une position en une position sous forme de string
         * \param pos Position à translater
         * \return La position sous forme de string
         *
         * Si le mode du tracker est HLineRubberBand/VLineRubberBand le label
         * retourné contient uniquement la valeur de la position en y/x. Dans
         * tous les autres cas, le label contient la position en x et en y.
         */
        virtual QwtText trackerTextF(const QPointF& pos) const;

    signals:

        /*!
         * \brief Signal émis à chaque déplacement de la souris.
         * \param pos Position de la souris
         */
        void mousePosChanged(const QPointF& pos) const;
};

#endif /* __ZOOMER_HPP__ */
