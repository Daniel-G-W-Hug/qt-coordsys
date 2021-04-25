#include "w_statusbar.hpp"

#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>

#include <cmath> // for axis scaling (and mathematical functions)

w_Statusbar::w_Statusbar(int width, QWidget* parent)
    : QWidget(parent), w_width(width), m_step(0) {

  setMinimumSize(w_width, w_height);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  updateGeometry();
}

void w_Statusbar::resizeEvent(QResizeEvent* event) {
  QSize currentSize = event->size();
  // only update changed width since heigth is kept constant
  if (currentSize.width() != w_width) {
    w_width = currentSize.width();
  }
}

void w_Statusbar::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e);
  QPainter qp(this);
  draw(&qp);
}

void w_Statusbar::draw(QPainter* qp) {

  const int nypos = 14; // ypos of all displayed strings

  // define logical coordinates (start with (0,0) in upper left corner)
  qp->setWindow(QRect(0, 0, w_width, w_height));

  qp->save();

  qp->setBrush(QBrush(QColor(Qt::lightGray)));
  qp->setPen(QPen(Qt::lightGray, 1, Qt::SolidLine));
  qp->drawRect(0, 0, w_width, w_height);

  qp->setFont(QFont("Helvetica", 12, QFont::Normal));
  qp->setPen(QPen(Qt::black, 1, Qt::SolidLine));
  QFontMetrics fm = qp->fontMetrics();

  if (m_hot) {
    QString x = QString::number(m_x, 'g', 3);
    QString y = QString::number(m_y, 'g', 3);
    QString s = QString("(x = ") + x + QString(", y = ") + y + QString(")");
    qp->drawText(w_width / 2 - fm.horizontalAdvance(s) / 2, nypos, s);
  }

  QString step = QString("Model: ") + QString::number(m_step);
  qp->drawText(w_width - fm.horizontalAdvance(step) - 10, nypos, step);

  qp->restore();
}

void w_Statusbar::on_mouseMoved(bool hot, double x, double y) {

  if (m_x != x || m_y != y || m_hot != hot) {
    // update only if any value has changed
    // fmt::print("received event: {} {} {}\n", hot, x, y);
    m_hot = hot;
    m_x = x;
    m_y = y;
    update();
  }
}

void w_Statusbar::on_modelChanged(int step) {

  if (m_step != step) {
    // update only if any value has changed
    // fmt::print("received event: {}\n", step);
    m_step = step;
    update();
  }
}