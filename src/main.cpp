#include "w_cs_view.hpp"

#include <QApplication>
#include <vector>

Coordsys make_cs() {
  axis_data ax, ay;
  ax.min = -2.;
  ax.max = 2.;
  ax.direction = Direction::x;
  // ax.scaling = Scaling::logarithmic;
  ax.label = "x label";
  ax.major_anchor = 0.0;
  ax.major_delta = 1.0;
  ax.minor_intervals = 4;

  ay.min = -1.1;
  ay.max = 1.1;
  ay.major_anchor = 0.0;
  ay.major_delta = 1.0;
  ay.minor_intervals = 10;

  // ay.min = -2;
  // ay.max = 2;
  // ay.scaling = Scaling::logarithmic;

  ay.direction = Direction::y;
  ay.label = "y label";

  widget_axis_data wx, wy;
  wx.w_size = 600;
  wx.a_length = 520;
  wx.a_offset = 50;

  wy.w_size = 400;
  wy.a_length = 320;
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

  return cm;
}

std::vector<Coordsys_model> make_vector_of_models() {

  std::vector<Coordsys_model> vm;

  line2d_mark lm;
  lm.pen = QPen(Qt::red, 2, Qt::SolidLine);
  // lm.mark_pts = true;
  lm.pm.symbol = Symbol::circle;
  lm.pm.pen = QPen(Qt::green, 1, Qt::SolidLine);

  double tmin = 0.0;
  double tmax = 2.0;
  double dt = 0.02;
  double t_eps = 0.01 * dt;

  double T = 1.0;
  double lambda = 2.0;
  //
  double omega = 2. * M_PI / T;
  double k = 2. * M_PI / lambda;

  double xmin = -2.0;
  double xmax = 2.0;
  double dx = 0.05;
  double x_eps = 0.01 * dx;

  // rechtslaufende Welle (Schwingungsdauer T, Wellenlänge lambda)
  // y(x,t) = y_max * sin(2*pi*(t/T - x/lambda))
  // y(x,t) = y_max * sin(omega*t - k*x)
  //
  // es gelten: omega = 2*pi/T; f = 1/T; Wellenzahl k = 2*pi/lambda
  // Ausbreitungsgeschwindigkeit: c = lambda*f => c*T = lambda

  for (double t = tmin; t <= tmax + t_eps; t += dt) {

    // define a poly line and a model
    line2d l;
    Coordsys_model cm;

    for (double x = xmin; x <= xmax + x_eps; x += dx) {
      l.push_back(pt2d(x, std::sin(omega * t - k * x)));
    }

    // add the poly line to the model
    cm.add_l(l, lm);

    // store the model in the vector
    vm.push_back(cm);
  }

  return vm;
}

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  Coordsys cs = make_cs();

  // single model case
  // Coordsys_model cm = make_model();
  // w_Cs_view window(&cs, &cm);

  // multi model case
  std::vector<Coordsys_model> vmodels;
  vmodels = make_vector_of_models();

  std::vector<Coordsys_model*> vm;
  for (int i = 0; i < vmodels.size(); ++i) {
    vm.push_back(&vmodels[i]);
  }

  w_Cs_view window(&cs, vm);

  // window.resize(600, 600);
  window.setWindowTitle("Coordsys");
  window.show();

  return app.exec();
}
