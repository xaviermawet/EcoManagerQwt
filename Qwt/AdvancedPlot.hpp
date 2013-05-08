#ifndef __ADVANCEPLOT_HPP__
#define __ADVANCEPLOT_HPP__

#include "Plot.hpp"
#include "../Utils/ColorPicker.hpp"
#include "TrackPlotCurve.hpp"
#include <qwt_picker_machine.h>

#define DEFAULT_NB_COLOR 8

class AdvancedPlot : public Plot
{
    Q_OBJECT

    public:

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param parent Widget parent
         */
        explicit AdvancedPlot(QString const& title,
                              int nbColor = DEFAULT_NB_COLOR,
                              QWidget* parent = NULL);

        /*!
         * \brief Constructeur
         * \param title Titre
         * \param parent Widget parent
         */
        explicit AdvancedPlot(QwtText const& title,
                              int nbColor = DEFAULT_NB_COLOR,
                              QWidget* parent = NULL);

        /*!
         * \brief Destructeur
         */
        virtual ~AdvancedPlot(void);

        /*!
         * \brief Ajoute une courbe de couleur aléatoire au graphique
         * \param title Le titre donné à la courbe
         * \param points Les coordonnées de tous les points de la courbe
         * \return Un pointeur vers la courbe nouvellement ajoutée
         */
        virtual QPlotCurve* addCurve(QString const& title,
                                     QVector<QPointF> const& points);

    signals:

        void intervalSelected(float minX, float maxX, QVariant trackId);

    protected slots:

        void pointsSelected(QRectF const& selectedRect);
        void pointSelected(QPointF const& point);

    protected:

        ColorPicker colorPicker;
};

#endif /* __ADVANCEPLOT_HPP__ */
