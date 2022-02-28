#include "w_cs_view.hpp"

#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>

#include <cmath> // for axis scaling (and mathematical functions)

w_Cs_view::w_Cs_view(Coordsys* cs, Coordsys_model* cm, QWidget* parent) :
    QWidget(parent)
{

    // set white as background color
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);

    wcs = new w_Coordsys(cs, cm, this);

    wsb = new w_Statusbar(cs->x.widget_size(), this);

    w1 = new QGroupBox;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(wcs);
    layout->addSpacing(5);
    layout->addWidget(wsb);
    setLayout(layout);

    // link coordsys to statusbar
    connect(wcs, SIGNAL(mouseMoved(bool, double, double)), wsb,
            SLOT(on_mouseMoved(bool, double, double)));
    connect(wcs, SIGNAL(modeChanged(pz_action, pz_mode)), wsb,
            SLOT(on_modeChanged(pz_action, pz_mode)));
    connect(wcs, SIGNAL(undoChanged(int)), wsb, SLOT(on_undoChanged(int)));
    connect(wcs, SIGNAL(labelChanged(std::string)), wsb,
            SLOT(on_labelChanged(std::string)));
    connect(wcs, SIGNAL(scalingChanged(axis_scal, axis_scal)), wsb,
            SLOT(on_scalingChanged(axis_scal, axis_scal)));

    // update status bar with label of first model
    emit wcs->labelChanged(cm->label());

    // update status bar with axis scaling
    emit wcs->scalingChanged(cs->x.scaling(), cs->y.scaling());
}

w_Cs_view::w_Cs_view(Coordsys* cs, const std::vector<Coordsys_model*> vm,
                     QWidget* parent) :
    QWidget(parent)
{

    // set white as background color
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);

    wcs = new w_Coordsys(cs, vm, this);

    wsb = new w_Statusbar(cs->x.widget_size(), this);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, vm.size() -
                            1); // only allow to switch between existing models

    w1 = new QGroupBox;
    // w1->setFlat(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(wcs);
    layout->addSpacing(5);
    layout->addWidget(slider);
    layout->addSpacing(5);
    // layout->addStretch();
    layout->addWidget(wsb);
    setLayout(layout);

    // link slider to model selection
    connect(slider, SIGNAL(valueChanged(int)), wcs, SLOT(switch_to_model(int)));
    connect(slider, SIGNAL(valueChanged(int)), wsb, SLOT(on_modelChanged(int)));
    // link coordsys to statusbar
    connect(wcs, SIGNAL(mouseMoved(bool, double, double)), wsb,
            SLOT(on_mouseMoved(bool, double, double)));
    connect(wcs, SIGNAL(modeChanged(pz_action, pz_mode)), wsb,
            SLOT(on_modeChanged(pz_action, pz_mode)));
    connect(wcs, SIGNAL(undoChanged(int)), wsb, SLOT(on_undoChanged(int)));
    connect(wcs, SIGNAL(labelChanged(std::string)), wsb,
            SLOT(on_labelChanged(std::string)));
    connect(wcs, SIGNAL(scalingChanged(axis_scal, axis_scal)), wsb,
            SLOT(on_scalingChanged(axis_scal, axis_scal)));

    // update status bar with label of first model
    emit wcs->labelChanged(vm[0]->label());

    // update status bar with axis scaling
    emit wcs->scalingChanged(cs->x.scaling(), cs->y.scaling());
}
