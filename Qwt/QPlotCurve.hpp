#ifndef __QPLOTCURVE_HPP__
#define __QPLOTCURVE_HPP__

#include <qwt_plot_curve.h>

/*!
 * \brief Courbe. Représente une série de points
 *
 * Une courbe est une représentation d'une série de points dans un plan x - y.
 */
class QPlotCurve : public QwtPlotCurve
{
    public:

        /*!
         * \brief Constructeur
         * \param title Titre donné à la courbe
         * \param pen Pinceau utilisé pour dessiner la courbe
         */
        explicit QPlotCurve(QString const& title, QPen const& pen);

        /*!
         * \brief Constructeur
         * \param title Titre donné à la courbe
         * \param pen Pinceau utilisé pour dessiner la courbe
         */
        explicit QPlotCurve(QwtText const& title, QPen const& pen);
};

#endif /* __QPLOTCURVE_HPP__ */
