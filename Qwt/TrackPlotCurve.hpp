#ifndef __TRACKPLOTCURVE_HPP__
#define __TRACKPLOTCURVE_HPP__

#include "QPlotCurve.hpp"
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>

typedef QMap<QString, QVariant> TrackIdentifier;

class TrackPlotCurve : public QPlotCurve
{
    public:

        /*!
           \brief Runtime type information

           RttiValues is used to cast plot items, without
           having to enable runtime type information of the compiler.
        */
        enum RttiValues
        {
            /*!
             * Values >= Rtti_PlotUserItem (1000) are reserved for plot items
             * not implemented in the Qwt library.
             */
            Rtti_TrackPlotCurveParent = 2001,
            Rtti_TrackPlotCurveChild = 2002
        };

        /*!
         * \brief Constructeur
         * \param title Titre donné à la courbe
         * \param trackId Identifiant unique d'un tour. Sera ajouté au titre
         * \param pen Pinceau utilisé pour dessiner la courbe
         */
        explicit TrackPlotCurve(QString const& title,
                                TrackIdentifier const& trackId,
                                QPen const& pen, TrackPlotCurve* parent = NULL);

        /*!
         * \brief Constructeur
         * \param title Titre donné à la courbe
         * \param trackId Identifiant unique d'un tour. Sera ajouté au titre
         * \param pen Pinceau utilisé pour dessiner la courbe
         */
        explicit TrackPlotCurve(QwtText const& title,
                                TrackIdentifier const& trackId,
                                QPen const& pen, TrackPlotCurve* parent = NULL);

        /*!
         * \brief Destructeur
         */
        virtual ~TrackPlotCurve(void);

        /*!
         * \brief Return rtti for the specific class represented.
         *
         *  The rtti value is useful for environments, where the runtime
         *  type information is disabled and it is not possible to do
         *  a dynamic_cast<...>.
         * \return la valeur du rtti
         */
        virtual int rtti(void) const;

        virtual void setColor(const QColor &color);
        virtual void setPointsColor(QColor const& color);

        /*!
         * \brief Obtenir l'identifiant unique (course + tour) auquel la courbe
         *        est associée
         * \return l'identifiant unique (course + tour)
         */
        TrackIdentifier trackIdentifier(void) const;

        /*!
         * \brief Retourne un pointeur vers la courbe parente
         * \return un pointeur vers la courbe parente
         */
        TrackPlotCurve const* parent(void) const;

        /*!
         * \brief Obtenir le rectangle de délimitation de la courbe courante et
         *        ses enfants
         * \return Le rectangle de délimitation de l'ensemble des courbes
         */
        virtual QRectF boundingRect(void) const;

        /*!
         * \brief Affiche ou masque la courbe courante et ses courbes enfant
         * \param visible true pour afficher les courbes, false pour les masquer
         */
        virtual void setVisible(bool visible);

        /*!
         * \brief Attache une courbe (enfant) à la courbe courante
         * \param curve la courbe enfant à attacher à la courbe courante
         */
        void attachChild(TrackPlotCurve* child);

        /*!
         * \brief Attache la courbe courante à une courbe parente.
         *        Si la parent vaut NULL, la courbe courante est simplement
         *        détachée de sont éventuel parent courant
         * \param parent Courbe à laquelle attacher la courbe courante
         */
        void attachTo(TrackPlotCurve* parent);

        /*!
         * \brief Détache la courbe courante de sa courbe parente
         */
        void detachFromParentCurve(void);

        /*!
         * \brief Détache la courbe enfant de la courbe courante
         * \param child
         */
        bool removeChild(TrackPlotCurve* const& child);

        void clearChildren(void);

        /*!
         * \brief Attache la courbe courante et tous ses enfants à un plot
         * \param plot graphique auquel attacher la courbe courante et ses
         *        courbes enfant
         */
        void attach(QwtPlot *plot);

        /*!
         * \brief Détache la courbe courante et tous ses enfants du graphique
         */
        void detach(void);

        void addPoint(qreal x);
        void addSector(qreal minX, qreal maxX);

        /*!
         * \brief Obtenir le point le plus proche d'un point donné
         * \param x valeur de l'abscisse
         * \return Le point le plus proche
         */
        QPointF closestPointF(QPointF const& pos, double* dist = NULL) const;

        QPointF closestPointFOfX(qreal const& posX, double* dist = NULL) const;

    protected:

        /*!
         * \brief Identifiant unique d'un tour
         */
        TrackIdentifier _trackIdentifier;

        /*!
         * \brief Courbe associées
         *
         *  Liste des courbes associées à la courbe courante. Ces courbes
         *  seront masquée/détruites en meme temps que leur parent.
         *  Ces courbes ont également la particularité de ne pas avoir
         *  d'occurence dans la légende
         */
        QList<TrackPlotCurve*> children;

        QList<QwtPlotMarker*> points;

        /*!
         * \brief Courbe parente
         */
        TrackPlotCurve* parentCurve;
};

#endif /* __TRACKPLOTCURVE_HPP__ */
