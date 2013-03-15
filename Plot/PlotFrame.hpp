#ifndef __PLOTFRAME_HPP__
#define __PLOTFRAME_HPP__

#include "PlotScene.hpp"
#include "PlotView.hpp"
#include "VerticalScale.hpp"
#include "HorizontalScale.hpp"
#include <QtGui>

namespace Ui {
class PlotFrame;
}

class PlotFrame : public QFrame
{
    Q_OBJECT
    
    public:

        explicit PlotFrame(QWidget *parent = 0);
        ~PlotFrame(void);

        // Getter
        PlotView*  view(void) const;
        PlotScene* scene(void) const;

        void addVerticalAxis(VerticalScale* scale);
        void addHorizontalAxis(HorizontalScale* scale);

    signals:

        void clear(void);
        void minVChanged(float);
        void maxVChanged(float);
        void minHChanged(float);
        void maxHChanged(float);

    private slots:

        // Autoconnect
        void on_eraseToolButton_clicked(void);
        void on_showLineToolButton_toggled(bool checked);
        void on_printToolButton_clicked(void);
        void on_eraseSelectionToolButton_clicked(void);
        void on_zoomSlider_valueChanged(int value);

        // Personal slots
        void adaptScales(const QRectF& newRect);
        void zoomAround(int level);

    protected:

        // GUI
        Ui::PlotFrame* ui;
        QVBoxLayout* topScaleLayout;
        QVBoxLayout* bottomScaleLayout;
        QHBoxLayout* leftScaleLayout;
        QHBoxLayout* rightScaleLayout;

        // Scene and view
        PlotScene* plotScene;
        PlotView*  plotView;
};

#endif /* __PLOTFRAME_HPP__ */
