#ifndef __ABSTRACTDOUBLEAXISPLOT_HPP__
#define __ABSTRACTDOUBLEAXISPLOT_HPP__

#include "Plot.hpp"
#include "Zoomer.hpp"

/*!
 * \brief Classe de base pour tout graphique 2D à systèmes d'axes multiples
 *
 *  AbstractDoubleAxisPlot est une classe abstraite, base pour tout graphique
 *  2D possédant deux axes des abscisses et/ou deux axes des ordonnées
 */
class AbstractDoubleAxisPlot : public Plot
{
    Q_OBJECT

    public:

        /*!
         * \brief Constructeur
         * \param title Titre du graphique
         * \param parent Widget parent du graphique
         */
        explicit AbstractDoubleAxisPlot(
            QString const& title, QWidget* parent = NULL);

        /*!
         * \brief Constructeur
         * \param title Titre du graphique
         * \param parent Widget parent du graphique
         */
        explicit AbstractDoubleAxisPlot(
            QwtText const& title, QWidget* parent = NULL);

        /*!
         * \brief Destructeur
         */
        virtual ~AbstractDoubleAxisPlot(void);

        /*!
         * \brief Méthode virtuelle pure. Méthode qui réalise un zoom sur
         *        un élément graphique
         * \param item Elément du graphique sur lequel on désire zoomer
         */
        virtual void zoom(QwtPlotItem const* item) = 0;

    protected:

        /*!
         * \brief Réalise les opérations de zoom pour le nouveau système d'axes.
         *
         * Adapte l'axe des abscisses du haut et l'axe des ordonnées de droite.
         */
        Zoomer* _xTopYRightZoomer;
};

#endif /* __ABSTRACTDOUBLEAXISPLOT_HPP__ */
