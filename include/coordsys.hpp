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

struct axis_data {

  // must contain scaled limits, i.e.
  // linear: min, max
  // logarithmic: log10(min), log10(max)
  double min{0.0};
  double max{10.0};

  Direction direction{Direction::undefined};
  Scaling scaling{Scaling::linear};
  std::string label{""};

  double major_anchor{0.0}; // anchor for major intervals
                            // i.e. point on axis major
                            // intervals are centered around
                            //
                            // ignored for log scale

  double major_delta{1.0}; // distance between two major notches
                           // each enclosed with major notches (0: none)
                           // major notches will be numbered
                           //
                           // ignored for log scale

  int minor_intervals{2}; // number of intervals within each major interval
                          // minor notches will not be numbered (0: none)
                          //
                          // ignored for log scale
};

class Axis // defines axis and scaling transformation to paint device
           // coordinates layout see Stroustrup, "Programming, Principles and
           // Practice using C++", p. 530ff (setup on p. 542)

// axis values are always to be regarded as scaled values
// (e.g. on logarithmic axis)
// unscaled means original values without axis scaling
{
public:
  Axis(widget_axis_data wd, axis_data ad);

  int a_to_w(
      double scaled_value) const; // (scaled) axis to widget transformation
  int au_to_w(
      double unscaled_value) const; // unscaled axis to widget transformation
  double w_to_a(int npos) const;    // widget to (scaled) axis transformation
  double w_to_au(int npos) const;   // widget to unscaled axis transformation

  void draw(QPainter* qp, int offset);

  double min() const { return ad.min; } // min as scaled value
  double max() const { return ad.max; } // max as scaled value
  double major_delta() const { return ad.major_delta; }
  int nmin() const { return a_to_w(ad.min); }
  int nmax() const { return a_to_w(ad.max); }
  int widget_size() const { return wd.w_size; }
  Scaling scaling() const { return ad.scaling; }
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
  void adjust_to_wheel_zoom(double new_xmin, double new_xmax, double new_ymin,
                            double new_ymax, double ref_xmin, double ref_xmax,
                            double ref_ymin, double ref_ymax, double ref_xdelta,
                            double ref_ydelta);

  Axis x;
  Axis y;

protected:
  double get_new_delta(double min, double max, double delta, double new_min,
                       double new_max);
  double get_new_delta_wheel_zoom(double min, double max, double delta,
                                  double new_min, double new_max,
                                  double ref_min, double ref_max,
                                  double ref_delta);

private:
  coordsys_data cd;

  // title as qt-String
  QString title;
};