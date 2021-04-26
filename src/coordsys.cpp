#include "coordsys.hpp"

#include <QPainter>
#include <QPalette>
#include <QString>
#include <QWidget>

#include <algorithm> // std::reverse
#include <cmath>     // for axis scaling (and mathematical functions)
#include <stdexcept>
#include <string>
#include <vector>

#include "fmt/format.h"
#include "fmt/ranges.h"

Axis::Axis(widget_axis_data wd, axis_data ad) : wd(wd), ad(ad) {
  // assure that preconditions are met
  if (ad.max <= ad.min)
    throw std::runtime_error("Requires axis_max > axis_min length).");
  if (wd.w_size < wd.a_offset + wd.a_length)
    throw std::runtime_error("Wiget size to small for axis and border.");
  if (wd.w_size <= 0)
    throw std::runtime_error("Widget size must be a positive value.");
  if (wd.a_offset <= 0)
    throw std::runtime_error("Border size must be a positive value.");
  if (wd.a_length <= 0)
    throw std::runtime_error("Axis length must be a positive value.");

  label = ad.label.c_str();

  switch (ad.direction) {
    case Direction::undefined:
      {
        throw std::runtime_error("Axis direction must be defined.");
        break;
      }
    case Direction::x:
      {
        // growing axis direction aligned with growing direction on paint device
        mo = wd.a_offset;
        sf = wd.a_length / (ad.max - ad.min);
        break;
      }
    case Direction::y:
      {
        // growing axis direction not aligned with growing direction on paint
        // device - store min_offset on paint device
        mo = wd.w_size - wd.a_offset;
        sf = -wd.a_length / (ad.max - ad.min);
        break;
      }
  }
}

// axis to widget transformation
int Axis::to_w(double axis_value, bool scaled) const {
  if (!scaled) {
    return sf * (axis_value - ad.min) + mo;
  } else {
    switch (ad.scaling) {
      case Scaling::linear:
        {
          return sf * (axis_value - ad.min) + mo;
          break;
        }
      case Scaling::logarithmic:
        {
          return sf * (std::log10(axis_value) - ad.min) + mo;
          return 0;
          break;
        }
    }
  }
}

// widget to axis transformation (noch überprüfen!)
double Axis::to_a(int nvalue, bool scaled) const {
  if (!scaled) {
    return (nvalue - mo) / sf + ad.min;
  } else {
    switch (ad.scaling) {
      case Scaling::linear:
        {
          return (nvalue - mo) / sf + ad.min;
          break;
        }
      case Scaling::logarithmic:
        {
          return std::pow(10, (nvalue - mo) / sf + ad.min);
          return 0;
          break;
        }
    }
  }
}

void Axis::draw(QPainter* qp, int offset) {

  // Create font
  qp->setFont(QFont("Helvetica", 12, QFont::Normal));
  // Set font color
  // qp->setPen(Qt::white);
  // Get QFontMetrics reference
  QFontMetrics fm = qp->fontMetrics();

  bool scaled = false;

  switch (ad.direction) {
    case Direction::x:
      {
        // axis
        qp->drawLine(nmin(), offset, nmax(), offset);

        // major notches
        std::vector<double> major_val = get_major_pos();
        for (int i = 0; i < major_val.size(); ++i) {
          int npos = to_w(major_val[i], scaled);
          qp->drawLine(npos, offset, npos, offset + 8);
          // notch labels
          QString s = QString::number(major_val[i]);
          qp->drawText(npos - fm.horizontalAdvance(s) / 2,
                       offset + fm.height() + 6, s);
        }
        // minor notches (w/o notch labels)
        std::vector<double> minor_val = get_minor_pos(major_val);
        for (int i = 0; i < minor_val.size(); ++i) {
          int npos = to_w(minor_val[i], scaled);
          qp->drawLine(npos, offset, npos, offset + 5);
        }

        // x axis label
        qp->save();
        qp->setFont(QFont("Helvetica", 14, QFont::Bold));
        QFontMetrics fmbx = qp->fontMetrics();
        qp->drawText((ad.max - ad.min) / 2 * sf + mo -
                         fmbx.horizontalAdvance(label) / 2,
                     offset + fmbx.height() + 20, label);
        qp->restore();

        // notify user on log10 scaling of axis
        if (ad.scaling == Scaling::logarithmic) {
          qp->save();
          qp->setFont(QFont("Helvetica", 14, QFont::Normal));
          QFontMetrics fmbx = qp->fontMetrics();
          qp->drawText((ad.max - ad.min) * sf + mo -
                           fmbx.horizontalAdvance(QString("log10(x)")),
                       offset + fmbx.height() + 20, QString("log10(x)"));
          qp->restore();
        }

        break;
      }

    case Direction::y:
      {
        // axis
        qp->drawLine(offset, nmin(), offset, nmax());

        // major notches
        std::vector<double> major_val = get_major_pos();
        for (int i = 0; i < major_val.size(); ++i) {
          int npos = to_w(major_val[i], scaled);
          qp->drawLine(offset - 8, npos, offset, npos);
          // notch labels
          QString s = QString::number(major_val[i]);
          qp->drawText(offset - fm.horizontalAdvance(s) - 11,
                       npos + fm.height() / 3, s);
        }
        // minor notches (w/o notch labels)
        std::vector<double> minor_val = get_minor_pos(major_val);
        for (int i = 0; i < minor_val.size(); ++i) {
          int npos = to_w(minor_val[i], scaled);
          qp->drawLine(offset - 5, npos, offset, npos);
        }

        // y axis label
        qp->save();
        qp->setFont(QFont("Helvetica", 14, QFont::Bold));
        QFontMetrics fmby = qp->fontMetrics();
        qp->translate(offset - fmby.height() - 20,
                      (ad.max - ad.min) * sf / 2 + mo -
                          fmby.horizontalAdvance(label) / 2);
        qp->rotate(90);
        qp->drawText(0, 0, label);
        qp->restore();

        // notify user on log10 scaling of axis
        if (ad.scaling == Scaling::logarithmic) {
          qp->save();
          qp->setFont(QFont("Helvetica", 14, QFont::Normal));
          QFontMetrics fmby = qp->fontMetrics();
          qp->translate(offset - fmby.height() - 20,
                        (ad.max - ad.min) * sf + mo);
          qp->rotate(90);
          qp->drawText(0, 0, QString("log10(y)"));
          qp->restore();
        }

        break;
      }
    case Direction::undefined:
      { // just to avoid warnings (case intercepted in ctor)
        break;
      }
  }
}

std::vector<double> Axis::get_major_pos() const {
  // return coodinates of major axis notches
  std::vector<double> notches;

  // combine sweep left and sweep right starting from anchor point

  switch (ad.scaling) {

    case Scaling::linear:
      {
        double value = ad.major_anchor;
        while (value >= ad.min) {                   // sweep left
          if (value >= ad.min && value <= ad.max) { // in interval now
            notches.push_back(value);
          }
          value -= ad.major_delta;
        }
        // reverse vector
        std::reverse(notches.begin(), notches.end());

        value = ad.major_anchor;
        while (value <= ad.max) { // sweep right
          if (value >= ad.min && value <= ad.max &&
              value != ad.major_anchor) { // in interval now (w/o anchor point)
            notches.push_back(value);
          }
          value += ad.major_delta;
        }
        break;
      }
    case Scaling::logarithmic: // ignore user settings of anchor, major_delta
                               // and minor_intervals and create standardized
                               // log10 axis
      {
        double value = 1.0;                         // standard anchor value
        while (value >= ad.min) {                   // sweep left
          if (value >= ad.min && value <= ad.max) { // in interval now
            notches.push_back(value);
          }
          value -= 1.0;
        }
        // reverse vector
        std::reverse(notches.begin(), notches.end());

        value = 1.0;              // standard anchor value
        while (value <= ad.max) { // sweep right
          if (value >= ad.min && value <= ad.max &&
              value != 1.0) { // in interval now (w/o anchor point)
            notches.push_back(value);
          }
          value += 1.0;
        }
        break;
      }
  }

  // fmt::print("major notches = {}\n", notches);
  return notches;
} // get_major_pos()

std::vector<double>
Axis::get_minor_pos(const std::vector<double>& major_pos) const {
  // return coodinates of minor axis notches
  // (excluding the position of the major notches)

  std::vector<double> notches;

  switch (ad.scaling) {

    case Scaling::linear:
      {

        { // anything before the leftmost major notch?
          double delta = (major_pos[1] - major_pos[0]) / ad.minor_intervals;
          if (ad.min <= major_pos[0] - delta) {
            double value_init = major_pos[0] - delta;
            double value = value_init;
            while (value >= ad.min) {                       // sweep left
              if (value >= ad.min && value <= value_init) { // in interval now
                notches.push_back(value);
              }
              value -= delta;
            }
            // reverse vector
            std::reverse(notches.begin(), notches.end());
          }
        }

        { // go through all major notches
          int last = major_pos.size() - 1;
          for (int i = 0; i < last; ++i) {
            double delta =
                (major_pos[i + 1] - major_pos[i]) / ad.minor_intervals;
            for (int j = 1; j < ad.minor_intervals;
                 ++j) { // skip the major notches
              notches.push_back(j * delta + major_pos[i]);
            }
          }
        }

        { // anything after the rightmost major notch?
          int last = major_pos.size() - 1;
          double delta =
              (major_pos[last] - major_pos[last - 1]) / ad.minor_intervals;
          if (ad.max >= major_pos[last] + delta) {
            double value_init = major_pos[last] + delta;
            double value = value_init;
            while (value <= ad.max) {                       // sweep right
              if (value <= ad.max && value >= value_init) { // in interval now
                notches.push_back(value);
              }
              value += delta;
            }
          }
        }

        break;
      }
    case Scaling::logarithmic:
      {

        { // anything before the leftmost major notch?
          double value = major_pos[0];
          double delta = std::pow(10, std::floor(value) - 1);
          for (int j = 0; j < 8; ++j) {
            value = std::log10(std::pow(10, value) - delta);
            if (value > ad.min) {
              notches.push_back(value);
            }
          }
          // reverse vector
          std::reverse(notches.begin(), notches.end());
        }

        { // go through all major notches
          int last = major_pos.size() - 1;
          for (int i = 0; i < last; ++i) {
            double value = major_pos[i];
            double delta = std::pow(10, std::floor(value));
            for (int j = 0; j < 8; ++j) {
              value = std::log10(std::pow(10, value) + delta);
              notches.push_back(value);
            }
          }
        }

        { // anything after the rightmost major notch?
          int last = major_pos.size() - 1;
          double value = major_pos[last];
          double delta = std::pow(10, std::floor(value));
          for (int j = 0; j < 8; ++j) {
            value = std::log10(std::pow(10, value) + delta);
            if (value < ad.max) {
              notches.push_back(value);
            }
          }
        }

        break;
      }
  }

  // fmt::print("minor notches = {}\n", notches);
  //   if (ad.direction == Direction::y)
  //     fmt::print("minor notches = {:.3f}\n", fmt::join(notches, ", "));
  return notches;
} // get_minor_pos()

Coordsys::Coordsys(Axis x, Axis y, coordsys_data cd)
    : x(x), y(y), cd(cd), title(cd.title.c_str()) {}

void Coordsys::draw(QPainter* qp) {

  qp->save();

  // draw helper lines
  qp->setPen(QPen(Qt::gray, 1, Qt::DotLine));
  bool scaled = false;

  { // draw helper lines through major notches
    std::vector<double> major_val = x.get_major_pos();
    for (int i = 0; i < major_val.size(); ++i) {
      int npos = x.to_w(major_val[i], scaled);
      qp->drawLine(npos, y.nmin(), npos, y.nmax());
    }
  }

  { // draw helper lines through major notches
    std::vector<double> major_val = y.get_major_pos();
    for (int i = 0; i < major_val.size(); ++i) {
      int npos = y.to_w(major_val[i], scaled);
      qp->drawLine(x.nmin(), npos, x.nmax(), npos);
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

  QRegion clip_area(
      QRect(x.nmin(), y.nmax(), x.nmax() - x.nmin(), y.nmin() - y.nmax()));
  qp->setClipRegion(clip_area);
}

void Coordsys::adjust_to_resized_widget(int new_w_width, int new_w_height) {

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

void Coordsys::adjust_to_pan(double dx, double dy) {

  if (dx != 0.0) {

    widget_axis_data wdx = x.get_widget_axis_data();
    axis_data adx = x.get_axis_data();

    // set new axis limits and create new axis
    adx.min -= dx;
    adx.max -= dx;
    x = Axis(wdx, adx);
  }

  if (dy != 0.0) {

    widget_axis_data wdy = y.get_widget_axis_data();
    axis_data ady = y.get_axis_data();

    // set new axis limits and create new axis
    ady.min -= dy;
    ady.max -= dy;
    y = Axis(wdy, ady);
  }
}