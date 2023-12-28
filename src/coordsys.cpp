#include "coordsys.hpp"

#include <QPainter>
#include <QPalette>
#include <QString>
#include <QWidget>

#include <algorithm> // std::reverse
#include <cmath> // for mathematical functions used for axis scaling (e.g. log10, pow, ceil)
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "fmt/format.h"
#include "fmt/ranges.h"

Axis::Axis(widget_axis_data wd_in, axis_data ad_in) : wd{wd_in}, ad{ad_in}
{

    // fmt::print("called ctor for axis\n");

    // assure that preconditions are met
    if (ad.rng.max <= ad.rng.min)
        throw std::runtime_error("Requires axis_max > axis_min length).");
    if (wd.w_size < wd.a_offset + wd.a_length)
        throw std::runtime_error("Wiget size to small for axis and border.");
    if (wd.w_size <= 0) throw std::runtime_error("Widget size must be a positive value.");
    if (wd.a_offset <= 0)
        throw std::runtime_error("Border size must be a positive value.");
    if (wd.a_length <= 0)
        throw std::runtime_error("Axis length must be a positive value.");

    label = ad.label.c_str();

    switch (ad.dir) {
        case axis_dir::x: {
            // growing axis direction aligned with growing direction on paint device
            mo = wd.a_offset;
            sf = wd.a_length / (ad.rng.max - ad.rng.min);
            break;
        }
        case axis_dir::y: {
            // growing axis direction not aligned with growing direction on paint
            // device - store min_offset on paint device
            mo = wd.w_size - wd.a_offset;
            sf = -wd.a_length / (ad.rng.max - ad.rng.min);
            break;
        }
    }

    // fmt::print("axis ctor: mo={}, sf={}\n", mo, sf);
}

// (scaled) axis to widget transformation
int Axis::a_to_w(double scaled_value) const
{
    return sf * (scaled_value - ad.rng.min) + mo;
}

// unscaled axis to widget transformation
int Axis::au_to_w(double unscaled_value) const
{
    switch (ad.scal) {
        case axis_scal::linear:

            return sf * (unscaled_value - ad.rng.min) + mo;
            break;

        case axis_scal::logarithmic:

            return sf * (std::log10(unscaled_value) - ad.rng.min) + mo;
            break;

        default:
#if defined(_MSC_VER)
            // MSVC is already conforming in C++23 mode
            std::unreachable();
#else
            // should work prior to gcc-12 and clang-15
            __builtin_unreachable();
#endif
    }
}

// widget to (scaled) axis transformation
double Axis::w_to_a(int npos) const { return (npos - mo) / sf + ad.rng.min; }

// widget to axis transformation
double Axis::w_to_au(int npos) const
{
    switch (ad.scal) {
        case axis_scal::linear:

            return (npos - mo) / sf + ad.rng.min;
            break;

        case axis_scal::logarithmic:

            return std::pow(10, (npos - mo) / sf + ad.rng.min);
            break;

        default:
#if defined(_MSC_VER)
            // MSVC is already conforming in C++23 mode
            std::unreachable();
#else
            // should work prior to gcc-12 and clang-15
            __builtin_unreachable();
#endif
    }
}

void Axis::draw(QPainter* qp, int offset)
{

    // Create font
    qp->setFont(QFont("Helvetica", 12, QFont::Normal));
    // Set font color
    // qp->setPen(Qt::white);
    // Get QFontMetrics reference
    QFontMetrics fm = qp->fontMetrics();

    switch (ad.dir) {
        case axis_dir::x: {
            // axis
            qp->drawLine(nmin(), offset, nmax(), offset);

            // major notches
            std::vector<double> major_val = get_major_pos();
            for (int i = 0; i < major_val.size(); ++i) {
                int npos = a_to_w(major_val[i]);
                if (npos >= nmin() && npos <= nmax()) { // just draw within cs area
                    qp->drawLine(npos, offset, npos, offset + 8);
                    // notch labels
                    QString s = QString::number(major_val[i]);
                    qp->drawText(npos - fm.horizontalAdvance(s) / 2,
                                 offset + fm.height() + 6, s);
                }
            }
            // minor notches (w/o notch labels)
            std::vector<double> minor_val = get_minor_pos(major_val);
            for (int i = 0; i < minor_val.size(); ++i) {
                int npos = a_to_w(minor_val[i]);
                if (npos >= nmin() && npos <= nmax()) { // just draw within cs area
                    qp->drawLine(npos, offset, npos, offset + 5);
                }
            }

            // x axis label
            qp->save();
            qp->setFont(QFont("Helvetica", 14, QFont::Bold));
            QFontMetrics fmbx = qp->fontMetrics();
            qp->drawText((ad.rng.max - ad.rng.min) / 2 * sf + mo -
                             fmbx.horizontalAdvance(label) / 2,
                         offset + fmbx.height() + 25, label);
            qp->restore();

            // notify user on log10 scaling of axis
            if (ad.scal == axis_scal::logarithmic) {
                qp->save();
                qp->setFont(QFont("Helvetica", 14, QFont::Normal));
                QFontMetrics fmbx = qp->fontMetrics();
                qp->drawText((ad.rng.max - ad.rng.min) * sf + mo -
                                 fmbx.horizontalAdvance(QString("log10(x)")),
                             offset + fmbx.height() + 25, QString("log10(x)"));
                qp->restore();
            }

            break;
        }

        case axis_dir::y: {
            // axis
            qp->drawLine(offset, nmin(), offset, nmax());

            // major notches
            std::vector<double> major_val = get_major_pos();
            for (int i = 0; i < major_val.size(); ++i) {
                int npos = a_to_w(major_val[i]);
                if (npos <= nmin() && npos >= nmax()) { // just draw within cs area (y!)
                    qp->drawLine(offset - 8, npos, offset, npos);
                    // notch labels
                    QString s = QString::number(major_val[i]);
                    qp->drawText(offset - fm.horizontalAdvance(s) - 11,
                                 npos + fm.height() / 3, s);
                }
            }
            // minor notches (w/o notch labels)
            std::vector<double> minor_val = get_minor_pos(major_val);
            for (int i = 0; i < minor_val.size(); ++i) {
                int npos = a_to_w(minor_val[i]);
                if (npos <= nmin() && npos >= nmax()) { // just draw within cs area (y!)
                    qp->drawLine(offset - 5, npos, offset, npos);
                }
            }

            // y axis label
            qp->save();
            qp->setFont(QFont("Helvetica", 14, QFont::Bold));
            QFontMetrics fmby = qp->fontMetrics();
            qp->translate(offset - fmby.height() - 30,
                          (ad.rng.max - ad.rng.min) * sf / 2 + mo +
                              fmby.horizontalAdvance(label) / 2);
            qp->rotate(-90);
            qp->drawText(0, 0, label);
            qp->restore();

            // notify user on log10 scaling of axis
            if (ad.scal == axis_scal::logarithmic) {
                qp->save();
                qp->setFont(QFont("Helvetica", 14, QFont::Normal));
                QFontMetrics fmby = qp->fontMetrics();
                qp->translate(offset - fmby.height() - 30,
                              (ad.rng.max - ad.rng.min) * sf + mo);
                qp->rotate(90);
                qp->drawText(0, 0, QString("log10(y)"));
                qp->restore();
            }

            break;
        }
    }
}

std::vector<double> Axis::get_major_pos() const
{
    // return coodinates of major axis notches
    std::vector<double> notches;

    // combine sweep left and sweep right starting from anchor point
    // sweep range is between ad.rng.min - ad.ticks.major_delta and ad.rng.max +
    // ad.ticks.major_delta to have at least the two major notches needed to draw the
    // minor notches

    switch (ad.scal) {

        case axis_scal::linear: {
            double value = ad.ticks.major_anchor;
            while (value >= ad.rng.min - ad.ticks.major_delta) { // sweep left
                if (value >= ad.rng.min - ad.ticks.major_delta &&
                    value <= ad.rng.max + ad.ticks.major_delta) { // in interval now
                    notches.push_back(value);
                }
                value -= ad.ticks.major_delta;
            }
            // reverse vector
            std::reverse(notches.begin(), notches.end());

            value = ad.ticks.major_anchor;
            while (value <= ad.rng.max + ad.ticks.major_delta) { // sweep right
                if (value >= ad.rng.min - ad.ticks.major_delta &&
                    value <= ad.rng.max + ad.ticks.major_delta &&
                    value !=
                        ad.ticks.major_anchor) { // in interval now (w/o anchor point)
                    notches.push_back(value);
                }
                value += ad.ticks.major_delta;
            }
            break;
        }
        case axis_scal::logarithmic: // ignore user settings of anchor, major_delta
                                     // and minor_intervals and create standardized
                                     // log10 axis
        {
            double value = 1.0;                 // standard anchor value
            while (value >= ad.rng.min - 1.0) { // sweep left
                if (value >= ad.rng.min - 1.0 &&
                    value <= ad.rng.max + 1.0) { // in interval now
                    notches.push_back(value);
                }
                value -= 1.0;
            }
            // reverse vector
            std::reverse(notches.begin(), notches.end());

            value = 1.0;                        // standard anchor value
            while (value <= ad.rng.max + 1.0) { // sweep right
                if (value >= ad.rng.min - 1.0 && value <= ad.rng.max + 1.0 &&
                    value != 1.0) { // in interval now (w/o anchor point)
                    notches.push_back(value);
                }
                value += 1.0;
            }
            break;
        }
    }

    // if (ad.dir == axis_dir::x) {
    //   fmt::print("x major notches = {}\n", notches);
    // }
    // if (ad.dir == axis_dir::y) {
    //   fmt::print("y major notches = {}\n", notches);
    // }
    return notches;
} // get_major_pos()

std::vector<double> Axis::get_minor_pos(const std::vector<double>& major_pos) const
{
    // return coodinates of minor axis notches
    // (excluding the position of the major notches)

    std::vector<double> notches;

    // the major_pos vector must at least contain two major notches so that we can
    // return something meaningful (otherwise the notches vector remains empty)
    if (major_pos.size() >= 2) {

        switch (ad.scal) {

            case axis_scal::linear: {
                // go through all major notches (there are at least two)
                int last = major_pos.size() - 1;
                for (int i = 0; i < last; ++i) {
                    double delta =
                        (major_pos[i + 1] - major_pos[i]) / ad.ticks.minor_intervals;
                    for (int j = 1; j < ad.ticks.minor_intervals;
                         ++j) { // skip the major notches
                        notches.push_back(j * delta + major_pos[i]);
                    }
                }
                break;
            }
            case axis_scal::logarithmic: {
                // go through all major notches (there are at least two)
                int last = major_pos.size() - 1;
                for (int i = 0; i < last; ++i) {
                    double value = major_pos[i];
                    double delta = std::pow(10, std::floor(value));
                    for (int j = 0; j < 8; ++j) {
                        value = std::log10(std::pow(10, value) + delta);
                        notches.push_back(value);
                    }
                }
                break;
            }
        }
    }

    // if (ad.dir == axis_dir::x) {
    //   fmt::print("x minor notches = {}\n", notches);
    //   fmt::print("x minor notches = {:.3f}\n", fmt::join(notches, ", "));
    // }
    // if (ad.dir == axis_dir::y) {
    //   fmt::print("y minor notches = {}\n", notches);
    //   // fmt::print("y minor notches = {:.3f}\n", fmt::join(notches, ", "));
    // }
    return notches;
} // get_minor_pos()

Coordsys::Coordsys(Axis x_in, Axis y_in, coordsys_data cd_in) :
    x{x_in}, y{y_in}, cd{cd_in}, title{cd.title.c_str()}
{
    // store target ratios once per Coordsys in order to allow for
    // scrollwheel scaling based on inital ratios set by user as target values
    // during inital configuration of coordinate system
    // (and not for each updated axis during zoom/pan/scale actions for axis)
    cd.x_rng_major_delta_target_ratio = (x.max() - x.min()) / x.major_delta();
    cd.y_rng_major_delta_target_ratio = (y.max() - y.min()) / y.major_delta();

    // fmt::print("Coordsys::Coordsys: x.target_ratio = {:.3}\n",
    // cd.x_rng_major_delta_target_ratio); fmt::print("Coordsys::Coordsys: y.target_ratio
    // = {:.3}\n\n", cd.y_rng_major_delta_target_ratio);
}

void Coordsys::draw(QPainter* qp)
{

    qp->save();

    // draw helper lines
    qp->setPen(QPen(Qt::gray, 1, Qt::DotLine));

    { // draw helper lines through major notches
        std::vector<double> major_val = x.get_major_pos();
        for (int i = 0; i < major_val.size(); ++i) {
            int npos = x.a_to_w(major_val[i]);
            if (npos >= x.nmin() && npos <= x.nmax()) { // just draw within cs area
                qp->drawLine(npos, y.nmin(), npos, y.nmax());
            }
        }
    }

    { // draw helper lines through major notches
        std::vector<double> major_val = y.get_major_pos();
        for (int i = 0; i < major_val.size(); ++i) {
            int npos = y.a_to_w(major_val[i]);
            if (npos <= y.nmin() && npos >= y.nmax()) { // just draw within cs area (y!)
                qp->drawLine(x.nmin(), npos, x.nmax(), npos);
            }
        }
    }

    qp->restore();

    // draw the axis, using the corresponding min values
    x.draw(qp, y.nmin());
    y.draw(qp, x.nmin());

    // make sure the outer frame is always drawn
    // regardless of the helper lines through the major notches
    qp->drawLine(x.nmin(), y.nmax(), x.nmax(), y.nmax());
    qp->drawLine(x.nmax(), y.nmin(), x.nmax(), y.nmax());

    // draw title
    qp->save();
    qp->setFont(QFont("Helvetica", 16, QFont::Bold));
    QFontMetrics fmbx = qp->fontMetrics();
    qp->drawText((x.nmax() + x.nmin()) / 2 - fmbx.horizontalAdvance(title) / 2,
                 y.nmax() - fmbx.height() / 2, title);
    qp->restore();

    // clipping area is active area of coordsys
    QRegion clip_area(
        QRect(x.nmin(), y.nmax(), x.nmax() - x.nmin(), y.nmin() - y.nmax()));
    qp->setClipRegion(clip_area);
}

void Coordsys::adjust_to_resized_widget(int new_w_width, int new_w_height)
{

    // needs adjustment of axis if corresponding widget size has changed
    if (new_w_width != x.widget_size()) {

        widget_axis_data wdx = x.get_widget_axis_data();
        axis_data adx = x.get_axis_data();

        // set new width and create new axis
        int delta_x = new_w_width - x.widget_size();
        wdx.w_size = new_w_width;
        wdx.a_length += delta_x;
        x = Axis(wdx, adx);
    }

    if (new_w_height != y.widget_size()) {

        widget_axis_data wdy = y.get_widget_axis_data();
        axis_data ady = y.get_axis_data();

        // set new width and create new axis
        int delta_y = new_w_height - y.widget_size();
        wdy.w_size = new_w_height;
        wdy.a_length += delta_y;
        y = Axis(wdy, ady);
    }
}

void Coordsys::adjust_to_pan(double dx, double dy)
{

    if (dx != 0.0) {

        widget_axis_data wdx = x.get_widget_axis_data();
        axis_data adx = x.get_axis_data();

        // set new axis limits and create new axis
        adx.rng.min -= dx;
        adx.rng.max -= dx;
        x = Axis(wdx, adx);
    }

    if (dy != 0.0) {

        widget_axis_data wdy = y.get_widget_axis_data();
        axis_data ady = y.get_axis_data();

        // set new axis limits and create new axis
        ady.rng.min -= dy;
        ady.rng.max -= dy;
        y = Axis(wdy, ady);
    }
}

void Coordsys::adjust_to_zoom(double new_xmin, double new_xmax, double new_ymin,
                              double new_ymax)
{

    // get data of existing axis
    widget_axis_data wdx = x.get_widget_axis_data();
    axis_data adx = x.get_axis_data();

    if (adx.rng.min != new_xmin || adx.rng.max != new_xmax) {
        // if there is some change
        // set new delta, as well as new min and new max
        adx.ticks.major_delta = get_new_delta(adx.rng.min, adx.rng.max,
                                              adx.ticks.major_delta, new_xmin, new_xmax);
        adx.rng.min = new_xmin;
        adx.rng.max = new_xmax;

        // create new axis
        x = Axis(wdx, adx);
    }

    widget_axis_data wdy = y.get_widget_axis_data();
    axis_data ady = y.get_axis_data();

    if (ady.rng.min != new_ymin || ady.rng.max != new_ymax) {
        // if there is some change
        // set new delta, as well as new min and new max
        ady.ticks.major_delta = get_new_delta(ady.rng.min, ady.rng.max,
                                              ady.ticks.major_delta, new_ymin, new_ymax);
        ady.rng.min = new_ymin;
        ady.rng.max = new_ymax;

        // create new axis
        y = Axis(wdy, ady);
    }
}

double Coordsys::get_new_delta(double min, double max, double delta, double new_min,
                               double new_max)
{

    // range r
    double r = max - min;

    // fmt::print("r = {}, dx = {}, n = {}, order of magn. = {}, delta_o = {}\n",
    // r,
    //            delta, int(r / delta), std::ceil(std::log10(r)),
    //            std::pow(10, std::ceil(std::log10(r))) / 10);

    double new_r = new_max - new_min;

    // how often does delta fit in new range:
    int new_n = new_r / delta;
    double fact = r / new_r;

    // fmt::print(
    //     "new_r = {},  dx = {}, new_n = {}, r/new_r = {}, int(r/new_r) = {}\n",
    //     new_r, delta, new_n, fact, int(r / new_r));

    double new_delta{delta};
    int factor{1};

    // only change delta if number of intervals in range changes significantly
    if ((new_n <= 2 || new_n >= 10) && (fact > 1.2)) {

        // round to nearest of 1,2,4,5,8,10,20,40,50,80,100,200
        if (fact > 1.2 && fact <= 3.) {
            factor = 2;
        }
        else if (fact > 3. && fact <= 4.5) {
            factor = 4;
        }
        else if (fact > 4.5 && fact <= 7.) {
            factor = 5;
        }
        else if (fact > 7. && fact <= 9.) {
            factor = 8;
        }
        else if (fact > 9. && fact <= 15.) {
            factor = 10;
        }
        else if (fact > 15. && fact <= 30.) {
            factor = 20;
        }
        else if (fact > 30. && fact <= 45.) {
            factor = 40;
        }
        else if (fact > 45. && fact <= 70.) {
            factor = 50;
        }
        else if (fact > 70. && fact <= 90.) {
            factor = 80;
        }
        else if (fact > 90. && fact <= 150.) {
            factor = 100;
        }
        else {
            factor = 200;
        }

        // fmt::print("fact = {}, factor = {}, ", fact, factor);

        new_delta /= factor;
    }

    // fmt::print("new_delta = {}\n\n", new_delta);

    return new_delta;
}

void Coordsys::adjust_to_wheel_zoom(double new_xmin, double new_xmax, double new_ymin,
                                    double new_ymax, double xtarget_ratio,
                                    double ytarget_ratio)
{

    // get data of existing axis
    widget_axis_data wdx = x.get_widget_axis_data();
    axis_data adx = x.get_axis_data();

    if (adx.rng.min != new_xmin || adx.rng.max != new_xmax) {
        // if something changed, create new axis

        adx.rng.min = new_xmin;
        adx.rng.max = new_xmax;

        adx.ticks.major_delta = get_new_delta_wheel_zoom(
            new_xmin, new_xmax, adx.ticks.major_delta, xtarget_ratio);

        x = Axis(wdx, adx);
    }

    widget_axis_data wdy = y.get_widget_axis_data();
    axis_data ady = y.get_axis_data();

    if (ady.rng.min != new_ymin || ady.rng.max != new_ymax) {
        // if something changed, create new axis

        ady.rng.min = new_ymin;
        ady.rng.max = new_ymax;

        ady.ticks.major_delta = get_new_delta_wheel_zoom(
            new_ymin, new_ymax, ady.ticks.major_delta, ytarget_ratio);

        y = Axis(wdy, ady);
    }
}

double Coordsys::get_new_delta_wheel_zoom(double new_min, double new_max, double delta,
                                          double target_ratio)
{

    double new_ratio = (new_max - new_min) / delta;

    double new_delta{delta};

    if (new_ratio >= 2. * target_ratio) new_delta *= 2.0;

    if (new_ratio <= 0.5 * target_ratio) new_delta *= 0.5;

    // fmt::print(
    //     "new_ratio = {:.3}, delta = {:.3}, target_ratio = {:.3}, new_delta =
    //     {:.3}\n\n", new_ratio, delta, target_ratio, new_delta);

    return new_delta;
}