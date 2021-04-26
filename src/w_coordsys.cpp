#include "w_coordsys.hpp"
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>

#include <cmath> // for axis scaling (and mathematical functions)

w_Coordsys::w_Coordsys(Coordsys* cs, Coordsys_model* cm, QWidget* parent)
    : QWidget(parent), cs(cs), cm(cm) {

  // store current model ptr in list of models (in case other models are added
  // later)
  vm.push_back(cm);

  setMinimumSize(cs->x.widget_size(), cs->y.widget_size());
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  updateGeometry();
}

w_Coordsys::w_Coordsys(Coordsys* cs, const std::vector<Coordsys_model*> vm,
                       QWidget* parent)
    : QWidget(parent), cs(cs), vm(vm) {

  // set current model ptr to first entry
  cm = vm[0];

  setMinimumSize(cs->x.widget_size(), cs->y.widget_size());
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  updateGeometry();

  // receive Mouse Move Events even when no button is pressed (default is false)
  // required to inform the status bar about the current mouse position
  setMouseTracking(true);
}

void w_Coordsys::resizeEvent(QResizeEvent* event) {
  QSize oldSize = event->oldSize();
  QSize currentSize = event->size(); // new widget size after resize
  if (oldSize != currentSize) {
    cs->adjust_to_resized_widget(currentSize.width(), currentSize.height());
  }
}

void w_Coordsys::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e);
  QPainter qp(this);
  qp.setRenderHint(QPainter::Antialiasing);
  draw(&qp);
}

void w_Coordsys::draw(QPainter* qp) {

  cs->draw(qp);
  cm->draw(qp, cs);
}

void w_Coordsys::mousePressEvent(QMouseEvent* event) {

  // accept mouse presses only in hot area
  if (event->button() == Qt::LeftButton && m_hot) {
    m_leftBotton = true;
  }
  if (event->button() == Qt::RightButton && m_hot) {
    m_rightBotton = true;
  }
}

void w_Coordsys::mouseReleaseEvent(QMouseEvent* event) {

  if (event->button() == Qt::LeftButton) {
    m_leftBotton = false;
  }
  if (event->button() == Qt::RightButton) {
    m_rightBotton = false;
  }
}

void w_Coordsys::mouseMoveEvent(QMouseEvent* event) {

  int nx = event->pos().x();
  int ny = event->pos().y();

  if (nx != m_nx || ny != m_ny) {
    // mouse moved to a new postion

    // convert coordinates
    double x_pos = cs->x.to_a(nx);
    double y_pos = cs->y.to_a(ny);

    // determine if mouse is in active cs area
    bool hot = false;
    if (x_pos >= cs->x.min() && x_pos <= cs->x.max() && y_pos >= cs->y.min() &&
        y_pos <= cs->y.max()) {
      hot = true;
    }
    m_hot = hot; // store whether mouse is still in hot area

    emit mouseMoved(hot, x_pos, y_pos);

    if (m_rightBotton && m_hot) {
      double dx = x_pos - cs->x.to_a(m_nx);
      double dy = y_pos - cs->y.to_a(m_ny);
      // fmt::print("dx = {}, dy = {}\n", dx, dy);
      cs->adjust_to_pan(dx, dy);
      update();
    }

    // store current position
    m_nx = nx;
    m_ny = ny;

    // fmt::print("m_nx = {}, m_ny = {}\n", m_nx, m_ny);
    // fmt::print("nx = {}, ny = {}\n", nx, ny);
    // fmt::print("delta_nx = {}, delta_ny = {}\n", delta_nx, delta_ny);

    // fmt::print("hot = {}, left = {}, right = {}\n", m_hot, m_leftBotton,
    //            m_rightBotton);

    // fmt::print("nx = {}, ny = {}, x_pos = {}, y_pos = {}\n", nx, ny, x_pos,
    //            y_pos);
  }
}

void w_Coordsys::switch_to_model(int idx) {

  if (idx >= 0 && idx < vm.size()) {
    // fmt::print("got signal {}\n", idx);
    cm = vm[idx];
    update();
  }
}
