#include "w_coordsys_view.hpp"
#include <QApplication>
#include <vector>

Coordsys make_cs() {
  axis_data ax, ay;
  ax.min = -4.5;
  ax.max = 4.5;
  ax.direction = Direction::x;
  // ax.scaling = Scaling::log10;
  ax.label = "x label";
  ax.major_anchor = 0.0;
  ax.major_delta = 1.0;
  ax.minor_intervals = 4;

  // ay.min = -5.0;
  // ay.max = 45.0;
  // ay.major_anchor = 0.0;
  // ay.major_delta = 5.0;
  // ay.minor_intervals = 5;

  ay.min = -2;
  ay.max = 2;
  ay.scaling = Scaling::log10;

  ay.direction = Direction::y;
  ay.label = "y label";

  widget_axis_data wx, wy;
  wx.w_size = 600;
  wx.a_length = 520;
  wx.a_offset = 50;

  wy.w_size = 600;
  wy.a_length = 520;
  wy.a_offset = 50;

  coordsys_data cd;
  cd.title = "Coordsys Title";

  Axis x(wx, ax);
  Axis y(wy, ay);
  Coordsys cs(x, y, cd);

  return cs;
}

Coordsys_model make_model() {

  Coordsys_model cm;

  pt2d p0;
  p0.y += 0.1;
  cm.add_p(p0);

  pt2d p1;
  p1.x = 3.0;
  p1.y = 2.0;
  cm.add_p(p1);

  pt2d p2(4.0, 3.0);
  pt2d_mark p2m;
  p2m.symbol = Symbol::circle;
  p2m.pen = QPen(Qt::green, 2, Qt::SolidLine);
  cm.add_p(p2, p2m);

  pt2d p3{4.0, 3.0};
  pt2d_mark p3m;
  p3m.symbol = Symbol::square;
  p3m.pen = QPen(Qt::red, 2, Qt::SolidLine);
  cm.add_p(p3, p3m);

  line2d l1;
  l1.push_back(p0);
  l1.push_back(p3);
  cm.add_l(l1);

  // Exponentialfunktion
  line2d l2;
  line2d_mark l2m;
  l2m.pen = QPen(Qt::red, 2, Qt::SolidLine);
  // l2m.mark_pts = true;
  // // l2m.delta = 2;
  // l2m.pm.symbol = Symbol::circle;
  // l2m.pm.pen = QPen(Qt::green, 1, Qt::SolidLine);

  double xmin = -4.0;
  double xmax = 4.0;
  double dx = 0.2;

  for (double x = xmin; x <= xmax; x += dx) {
    l2.push_back(pt2d(x, std::exp(x)));
  }

  cm.add_l(l2, l2m);

  return cm;
}

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  Coordsys cs = make_cs();
  Coordsys_model cm = make_model();
  w_Coordsys_view window(&cs, &cm);

  window.resize(600, 600);
  window.setWindowTitle("Coordsys");
  window.show();

  return app.exec();
}
