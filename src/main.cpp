#include "w_cs_view.hpp"

#include "hd/hd_functions.hpp"

#include <QApplication>
#include <exception>
#include <iostream>
#include <vector>

Coordsys make_cs()
{
    axis_data ax, ay;
    ax.min = -0.5;
    ax.max = 1.5;
    ax.direction = Direction::x;
    // ax.scaling = Scaling::logarithmic;
    ax.label = "x label";
    ax.major_anchor = 0.0;
    ax.major_delta = 1.0;
    ax.minor_intervals = 4;

    ay.min = -0.1;
    ay.max = 1.1;
    ay.major_anchor = 0.0;
    ay.major_delta = 1.0;
    ay.minor_intervals = 4;

    // ay.min = -2;
    // ay.max = 3;
    // ay.scaling = Scaling::logarithmic;

    // ay.min = 0;
    // ay.max = 1001;
    // ay.major_delta = 100.0;

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

Coordsys_model make_model()
{

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

    ln2d l1;
    l1.push_back(p0);
    l1.push_back(p3);
    cm.add_l(l1);

    {
        ln2d l2;
        double x = -0.5;
        double dx = 0.01;
        double x_eps = 0.1 * dx;

        while (x <= 1.5 + x_eps)
        {
            l2.push_back(pt2d(x, hd::linear_step(0.0, 1.0, x)));
            x += dx;
        }
        ln2d_mark l2m;
        l2m.mark_pts = true;
        // l2m.delta = 10; // show every tenth point only
        l2m.delta = 1;
        l2m.pm.symbol = circle;
        l2m.pm.pen = QPen(Qt::green, 2, Qt::SolidLine);
        cm.add_l(l2, l2m);
    }

    {

        ln2d l2;
        double x = -0.5;
        double dx = 0.01;
        double x_eps = 0.1 * dx;

        while (x <= 1.5 + x_eps)
        {
            l2.push_back(pt2d(x, hd::smooth_step(0.0, 1.0, x)));
            x += dx;
        }
        ln2d_mark l2m;
        l2m.mark_pts = true;
        l2m.delta = 10; // show every tenth point only
        l2m.pm.symbol = plus;
        l2m.pm.pen = QPen(Qt::cyan, 2, Qt::SolidLine);

        cm.add_l(l2, l2m);
    }

    {

        ln2d l2;
        double x = -0.5;
        double dx = 0.01;
        double x_eps = 0.1 * dx;

        while (x <= 1.5 + x_eps)
        {
            l2.push_back(pt2d(x, hd::smoother_step(0.0, 1.0, x)));
            x += dx;
        }
        ln2d_mark l2m;
        l2m.mark_pts = true;
        l2m.delta = 10; // show every tenth point only
        l2m.pm.symbol = square;
        l2m.pm.pen = QPen(Qt::blue, 2, Qt::SolidLine);

        cm.add_l(l2, l2m);
    }

    vec2d v1;
    v1.to.x = 2;
    v1.to.y = 3;
    cm.add_v(v1);

    cm.set_label("init label");

    return cm;
}

std::vector<Coordsys_model> make_vector_of_models()
{

    std::vector<Coordsys_model> vm;

    ln2d_mark lm;
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

    for (double t = tmin; t <= tmax + t_eps; t += dt)
    {

        // define a poly line and a model
        ln2d l;
        Coordsys_model cm;

        for (double x = xmin; x <= xmax + x_eps; x += dx)
        {
            l.push_back(pt2d(x, std::sin(omega * t - k * x)));
        }

        // add the poly line to the model
        cm.add_l(l, lm);

        cm.set_label(fmt::format("t={:.3f}", t));

        // store the model in the vector
        vm.push_back(cm);
    }

    return vm;
}

int main(int argc, char* argv[])
{

    try
    {
        QApplication app(argc, argv);

        Coordsys cs = make_cs();

        // fmt::print("Size of cs = {}\n", sizeof(cs));

        // single model case
        Coordsys_model cm = make_model();
        w_Cs_view window(&cs, &cm);

        // multi model case
        // std::vector<Coordsys_model> vmodels;
        // vmodels = make_vector_of_models();

        // std::vector<Coordsys_model*> vm;
        // for (int i = 0; i < vmodels.size(); ++i) {
        //   vm.push_back(&vmodels[i]);
        // }
        // w_Cs_view window(&cs, vm);

        // window.resize(600, 600);
        window.setWindowTitle("Coordsys");
        window.show();

        return app.exec();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }
}
