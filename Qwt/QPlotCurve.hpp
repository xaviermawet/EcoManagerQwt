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
            Rtti_CustomPlotCurve = 2000
        };

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

        /*!
         * \brief Return rtti for the specific class represented.
         *
         *  The rtti value is useful for environments, where the runtime
         *  type information is disabled and it is not possible to do
         *  a dynamic_cast<...>.
         * \return la valeur du rtti
         */
        virtual int rtti(void) const;
};

#endif /* __QPLOTCURVE_HPP__ */
