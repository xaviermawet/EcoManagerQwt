#ifndef __DOUBLEYAXISPLOT_HPP__
#define __DOUBLEYAXISPLOT_HPP__

#include "AbstractDoubleAxisPlot.hpp"
#include <qwt_plot_rescaler.h>

/*!
 * \brief Graphique 2D avec deux axes des ordonnées
 */
class DoubleYAxisPlot : public AbstractDoubleAxisPlot
{
    Q_OBJECT

    public:

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param aspectRatio Rapport d'échelle entre l'axe des ordonnées de
         *                    gauche et celui de droite
         * \param parent Widget parent
         */
        explicit DoubleYAxisPlot(
            QString const& title, double aspectRatio, QWidget* parent = NULL);

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param aspectRatio Rapport d'échelle entre l'axe des ordonnées de
         *                    gauche et celui de droite
         * \param parent Widget parent
         */
        explicit DoubleYAxisPlot(
            QwtText const& title, double aspectRatio, QWidget* parent = NULL);

        /*!
         * \brief Destructeur
         */
        virtual ~DoubleYAxisPlot(void);

        /*!
         * \brief Réalise un zoom sur un élément graphique
         * \param item Elément du graphique sur lequel on désire zoomer
         */
        virtual void zoom(QwtPlotItem const* item);

    protected:

        /*!
         * \brief Rapport d'échelle entre l'axe des ordonnées de gauche et l'axe
         *        des ordonnées de droite
         */
        double  _ratio;

        /*!
         * S'occupe de garder un rapport identique entre les deux axes des
         * ordonnées
         */
        QwtPlotRescaler* _yRescaler;
};

#endif /* __DOUBLEYAXISPLOT_HPP__ */
