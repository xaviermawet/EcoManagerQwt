/* TODO :
 * ------------------
 * Disons que le titre passé ne contient pas le numéro du tour, ni le
 * numéro de la course (attention c'est bien le numéro et pas l'id pour la
 * course) --> alors, dans le constructeur, ajouter au titre le numéro de
 * la course et le numéro du tour
 *
 * Modifier le boundingRect de la courbe pour qu'elle tienne compte de tous ses enfants
 *
 * Lorsqu'on attache une courbe enfant à un parent, il faut faire correspondre leur trackId
 *
 */
#ifndef __TRACKPLOTCURVE_HPP__
#define __TRACKPLOTCURVE_HPP__

#include "QPlotCurve.hpp"
#include <qwt_plot.h>

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
         * \brief Attache la courbe courante et tous ses enfants à un plot
         * \param plot graphique auquel attacher la courbe courante et ses
         *        courbes enfant
         */
        void attach(QwtPlot *plot);

        /*!
         * \brief Détache la courbe courante et tous ses enfants du graphique
         */
        void detach(void);

        virtual QRectF boundingRect(void) const;

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
