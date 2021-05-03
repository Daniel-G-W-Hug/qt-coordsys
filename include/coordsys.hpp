#pragma once

#include <QPainter>
#include <QString>
#include <QWidget>

#include <string>
#include <vector>

enum Direction { undefined, x, y };
enum Scaling { linear, logarithmic }; // tbd: date, time

struct widget_axis_data {
  int w_size{600};
  int a_offset{50};
  int a_length{520};
};

struct axis_data { // linear: min, max
                   // logarithmic: log10(min), log10(max)
  double min{0.0};
  double max{10.0};

  Direction direction{Direction::undefined};
  Scaling scaling{Scaling::linear};
  std::string label{""};

  double major_anchor{0.0}; // anchor for major intervals
                            // i.e. point on axis major intervals
                            // are centered around
                            //
                            // ignored for log10-scale

  double major_delta{1.0}; // distance between two major notches
                           // each enclosed with major notches (0: none)
                           // major notches will be numbered
                           //
                           // ignored for log10 scale
                           //
  int minor_intervals{2};  // number of intervals within each major interval
                           // minor notches will not be numbered (0: none)
                           //
                           // ignored for log10-scale
};

class Axis // defines axis and scaling transformation to paint device
           // coordinates layout see Stroustrup, "Programming, Principles and
           // Practice using C++", p. 530ff (setup on p. 542)
{
public:
  Axis(widget_axis_data wd, axis_data ad);

  int to_w(double value,
           bool scaled = true) const; // axis to widget transformation
  double to_a(int nvalue,
              bool scaled = true) const; // widget to axis transformation

  void draw(QPainter* qp, int offset);

  double min() const { return ad.min; }
  double max() const { return ad.max; }
  int nmin() const { return to_w(ad.min, false); }
  int nmax() const { return to_w(ad.max, false); }
  int widget_size() const { return wd.w_size; }
  widget_axis_data get_widget_axis_data() const { return wd; }
  axis_data get_axis_data() const { return ad; }

  std::vector<double> get_major_pos() const;
  std::vector<double> get_minor_pos(const std::vector<double>& major_pos) const;

private:
  widget_axis_data wd;
  axis_data ad;

  // axis label as qt-String
  QString label;

  // calculated values
  int mo;    // min offset position on paint device
  double sf; // scaling factor to map axis length and scaling direction to
             // length and direction on paint device
};

struct coordsys_data {
  std::string title{""};
};

class Coordsys {
public:
  Coordsys(Axis x, Axis y, coordsys_data cd);
  void draw(QPainter* qp);

  coordsys_data get_coordsys_data() const { return cd; }

  void adjust_to_resized_widget(int new_w_width, int new_w_height);
  void adjust_to_pan(double dx, double dy);
  void adjust_to_zoom(double new_xmin, double new_xmax, double new_ymin,
                      double new_ymax);

  Axis x;
  Axis y;

private:
  coordsys_data cd;

  // title as qt-String
  QString title;
};