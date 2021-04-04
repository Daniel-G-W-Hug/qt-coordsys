#include "w_coordsys_view.hpp"
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>

#include <cmath> // for axis scaling (and mathematical functions)

w_Coordsys_view::w_Coordsys_view(Coordsys* cs, Coordsys_model* cm,
                                 QWidget* parent)
    : QWidget(parent), cs(cs), cm(cm) {
  // set white as background color
  QPalette pal;
  pal.setColor(QPalette::Window, Qt::white);
  setAutoFillBackground(true);
  setPalette(pal);
}

void w_Coordsys_view::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e);
  QPainter qp(this);
  qp.setRenderHint(QPainter::Antialiasing);
  draw(&qp);
}

void w_Coordsys_view::draw(QPainter* qp) {

  cs->draw(qp);
  cm->draw(qp, cs);
}
