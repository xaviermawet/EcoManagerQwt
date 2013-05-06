#ifndef __TRACKPLOTCURVE_HPP__
#define __TRACKPLOTCURVE_HPP__

#include "QPlotCurve.hpp"

typedef QMap<QString, QVariant> TrackIdentifier;

class TrackPlotCurve : public QPlotCurve
{
    public:

        /*!
         * \brief Constructeur
         * \param title Titre donné à la courbe
         * \param trackId Identifiant unique d'un tour
         * \param pen Pinceau utilisé pour dessiner la courbe
         */
        explicit TrackPlotCurve(QString const& title,
                                TrackIdentifier const& trackId,
                                QPen const& pen, TrackPlotCurve* parent = NULL);

        /*!
         * \brief Constructeur
         * \param title Titre donné à la courbe
         * \param trackId Identifiant unique d'un tour
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
         * \brief Affiche ou masque la courbe
         * \param visible true permet d'afficher la courbe, false de la masquer
         */
        virtual void setVisible(bool visible);

        /*!
         * \brief Attache une courbe (enfant) à la courbe courante
         * \param curve la courbe enfant à attacher à la courbe courante
         */
        void attach(TrackPlotCurve* child);

        /*!
         * \brief Attache la courbe courante à une courbe parente
         * \param parent Courbe à laquelle attacher la courbe courante
         */
        void attachTo(TrackPlotCurve* parent);

        /*!
         * \brief Détache la courbe courante de sa courbe parente
         */
        void detachFromParentCurve(void);

    protected:

        void removeChild(TrackPlotCurve* const& child);

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

        /*!
         * \brief Courbe parente
         */
        TrackPlotCurve* parentCurve;
};

#endif /* __TRACKPLOTCURVE_HPP__ */
