#include "w_cs_view.hpp"

#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>

#include <cmath> // for axis scaling (and mathematical functions)

w_Cs_view::w_Cs_view(Coordsys* cs, Coordsys_model* cm, QWidget* parent)
    : QWidget(parent) {

  // set white as background color
  QPalette pal;
  pal.setColor(QPalette::Window, Qt::white);
  setAutoFillBackground(true);
  setPalette(pal);

  wcs = new w_Coordsys(cs, cm, this);
}

w_Cs_view::w_Cs_view(Coordsys* cs, const std::vector<Coordsys_model*> vm,
                     QWidget* parent)
    : QWidget(parent) {

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

  // connect slider to model selection
  connect(slider, SIGNAL(valueChanged(int)), wcs, SLOT(switch_to_model(int)));
  connect(slider, SIGNAL(valueChanged(int)), wsb, SLOT(on_modelChanged(int)));
  connect(wcs, SIGNAL(mouseMoved(bool, double, double)), wsb,
          SLOT(on_mouseMoved(bool, double, double)));
}