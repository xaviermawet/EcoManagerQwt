#ifndef __PLOTMAGNIFIER_HPP__
#define __PLOTMAGNIFIER_HPP__

#include <qwt_plot_magnifier.h>
#include <QDebug>

/*!
 * \brief Fournit un système de zoom pas à pas pour les Plot
 *
 * En utilisant un PlotMagnifier, on peut zoomer/dezoomer à l'aide de raccourcis
 * clavier, de la moletter de la souris ou en déplaceant verticalement le souris
 * tout en appuyant sur un des boutons
 */
class PlotMagnifier : public QwtPlotMagnifier
{
    Q_OBJECT

    public:

        /*!
         * \brief Constructeur
         * \param plotCanvas Toile d'un Plot
         */
        explicit PlotMagnifier(QwtPlotCanvas* plotCanvas);

        /*!
         * \brief Rétabli (la profondeur) le zoom à sont état initial
         */
        void restoreScale(void);

    signals:

        /*!
         * \brief Signal émis à chaque fois qu'un redimensionnement est réalisé
         * \param factor facteur de redimensionnement
         */
        void rescaled(double factor);

    public slots:

        /*!
         * \brief Réalise une redimention (zoom)
         * \param factor facteur de redimensionnement
         */
        virtual void doRescale(double factor);

    protected:

        /*!
         * \brief Réalise un zoom
         * \param factor facteur de redimensionnement
         *
         * Méthode surchargée de la classe QwtPlotMagnifier. Mémorise toutes
         * les étapes de redimensionnement afin de pouvoir restaurer la
         * profondeur du zoom à son état initial
         */
        virtual void rescale(double factor);

    protected:

        /*!
         * Renseigne le facteur de zoom resultant de tous les redimensionnement.
         * Est utilisé pour restaurer (la profondeur) le zoom à son état initial
         */
        double _factors;
};

#endif /* __PLOTMAGNIFIER_HPP__ */
