#ifndef __DOUBLEXAXISPLOT_HPP__
#define __DOUBLEXAXISPLOT_HPP__

#include "AbstractDoubleAxisPlot.hpp"
#include <qwt_plot_rescaler.h>

/*!
 * \brief Graphique 2D avec deux axes des abscisses
 */
class DoubleXAxisPlot : public AbstractDoubleAxisPlot
{
    Q_OBJECT

    public:

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param aspectRatio Rapport d'échelle entre l'axe des abscisses du bas
         *                    et celui du hauts
         * \param parent Widget parent
         */
        explicit DoubleXAxisPlot(
            QString const& title, double aspectRatio, QWidget* parent = NULL);

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param aspectRatio Rapport d'échelle entre l'axe des abscisses du bas
         *                    et celui du hauts
         * \param parent Widget parent
         */
        explicit DoubleXAxisPlot(
            QwtText const& title, double aspectRatio, QWidget* parent = NULL);

        /*!
         * \brief Destructeur
         */
        virtual ~DoubleXAxisPlot(void);

        /*!
         * \brief Réalise un zoom sur un élément graphique
         * \param item Elément du graphique sur lequel on désire zoomer
         */
        virtual void zoom(QwtPlotItem const* item);

    protected:

        /*!
         * \brief Rapport d'échelle entre l'axe des abscisses du bas et l'axe
         *        des abscisses du hauts
         */
        double           _ratio;

        /*!
         * \brief S'occupe de garder un rapport identique entre les deux axes
         *        des abscisses
         */
        QwtPlotRescaler* _xRescaler;
};

#endif /* __DOUBLEXAXISPLOT_HPP__ */
