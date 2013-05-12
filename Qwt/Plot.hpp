#ifndef __PLOT_HPP__
#define __PLOT_HPP__

#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_legend_item.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_magnifier.h>
#include "Zoomer.hpp"
#include "PlotMagnifier.hpp"
#include "QPlotCurve.hpp"

/*!
 * \brief Graphique 2D
 *
 * Plot est un graphique 2D. Ce graphique peut afficher un nombre illimité
 * d'éléments graphiques. Un élément graphique peut être soit une courbe
 * (PlotCurve), un marqueur (QwtPlotMarker), une grille (QwtPlotGrid), ou
 * n'importe quel autre objet dérivant de QwtPlotItem.
 * Un graphique peut avoir jusqu'à quatres axes (voir aussi DoubleYAxisPlot et
 * DoubleXAxisPlot) et chaque élément graphique est attaché à un axe x et un
 * axe y.
 */
class Plot : public QwtPlot
{
    Q_OBJECT

    public:

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param parent Widget parent
         */
        explicit Plot(QString const& title, QWidget* parent = NULL);

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param parent Widget parent
         */
        explicit Plot(QwtText const& title, QWidget* parent = NULL);

        /*!
         * \brief Destructeur
         */
        virtual ~Plot(void);

        /*!
         * \brief Réalise un zoom sur une zone rectangulaire
         * \param zoomRectF Zone rectangulaire sur laquelle on désirer zoomer
         */
        void zoom(QRectF const& zoomRectF);

        /*!
         * \brief Réalise un zoom sur un élément graphique
         * \param item Elément du graphique sur lequel on désire zoomer
         */
        virtual void zoom(QwtPlotItem const* item);

        /*!
         * \brief Ajoute une courbe au graphique
         * \param title Le titre donné à la courbe
         * \param points Les coordonnées de tous les points de la courbe
         * \param pen La couleur de la courbe
         * \return Un pointeur vers la courbe nouvellement ajoutée
         */
        virtual QPlotCurve* addCurve(QString const& title,
                                     QVector<QPointF> const& points,
                                     QPen const& pen);

        /* ------------------------------------------------------------------ *
         *                              Getters                               *
         * ------------------------------------------------------------------ */

        /*!
         * \brief Indique si la grille du graphique est visible
         * \return true si la grille est visible, false le cas contraire
         */
        bool isGridVisible(void) const;

        /*!
         * \brief Indique si la croix est visible
         * \return true si la croix est visible, false le cas contraire
         */
        bool isCrossLineVisible(void) const;

        /*!
         * \brief Indique si la le label renseignant la position de la souris
         *        est visible
         * \return true si le label est visible, false le cas contraire
         */
        bool isLabelPositionVisible(void) const;

        /*!
         * \return Un pointeur vers la grille du graphique
         */
        QwtPlotGrid*   grid(void) const;

        /*!
         * \return Un pointeur vers la croix du graphique
         */
        QwtPlotMarker* crossLine(void) const;

        /*!
         * \return Un pointeur vers l'objet réalisant un zoom pour l'axe des
         *         abscisse du bas et l'axe des ordonnées de gauche
         */
        Zoomer* zoomer(void) const;

    signals:

        /*!
         * \brief Signal émis lors d'un clic droit sur un élément de la légende
         * \param item Élément graphique que référence l'élément de la légende
         * \param pos Position à laquelle l'utilisateur a cliqué
         */
        void legendRightClicked(QwtPlotItem const* item, QPoint const& pos);

    public slots:

        /* ------------------------------------------------------------------ *
         *                          Setters - SLOTS                           *
         * ------------------------------------------------------------------ */

        /*!
         * \brief Modifie la visibilité de la grille
         * \param visible true pour rendre la grille visible,
         *        false le cas contraire
         */
        void setGridVisible(bool visible);

        /*!
         * \brief Modifie la visibilité de la croix
         * \param visible true pour rendre la croix visible,
         *        false le cas contraire
         */
        void setCrossLineVisible(bool visible);

        /*!
         * \brief Modifie la visibilité du label renseignant la position de la
         *        souris
         * \param visible true pour rendre le label visible,
         *        false le cas contraire
         */
        void setLabelPositionVisible(bool visible);

        virtual void clearcurves(void);

        virtual void changeForegroundColor(void);

        virtual void globalZoom(void);

    protected slots:

        /* ------------------------------------------------------------------ *
         *                          Personnal slots                           *
         * ------------------------------------------------------------------ */

        /*!
         * \brief Met à jours la position du label renseignant la position de la
         *        souris
         * \param pos Nouvelle position de la souris
         */
        void updateCrossLinePosition(QPointF const& pos);

        /*!
         * \param pos Position de la souris lors du clic droit
         *
         * Récupère le pointeur vers l'élément graphique qui est référencé par
         */
        void showLegendContextMenu(QPoint const& pos);

    protected:

        /*!
         * \brief Légende du graphique
         */
        QwtLegend*     _legend;

        /*!
         * \brief Grille du graphique
         */
        QwtPlotGrid*   _grid;

        /*!
         * \brief Marqueur en forme de croix
         */
        QwtPlotMarker* _crossLine;

        /*!
         * \brief Gère le deplacement des éléments graphiques (panning)
         */
        QwtPlotPanner* _panner;

        /*!
         * \brief Gère les opérations de zoom pour le système d'axes par defaut.
         *
         * Adapte l'axe des abscisses du bas et l'axe des ordonnées de gauche.
         */
        Zoomer*        _xBottomYLeftZoomer;

        /*!
         * \brief Gère le zoom à la molette et à l'aide de raccourcis clavier
         */
        PlotMagnifier* _magnifier;

        /*!
         * \brief Est utilisé pour sauvegarder/restaurer l'état de visibilité
         * du label avant d'afficher/de masquer la croix
         */
        bool _labelWasVisible;
};

#endif /* __PLOT_HPP__ */
