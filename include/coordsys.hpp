#pragma once

#include <QPainter>
#include <QString>
#include <QWidget>

#include <string>
#include <vector>

struct mouse_pos_t // mouse position in various systems
{
    int nx, ny;  // pos in device coordinate system
    double x, y; // pos in coordinate system
};

struct axis_rng // range covered by axis from min to max
{
    axis_rng() = default;
    axis_rng(double min_in, double max_in) : min{min_in}, max{max_in} {}
    axis_rng(const axis_rng& other) = default;
    axis_rng& operator=(const axis_rng& other) = default;
    axis_rng(axis_rng&& other) = default;
    axis_rng& operator=(axis_rng&& other) = default;

    double min{0.0};
    double max{1.0};
};

enum axis_dir // direction of axis
{
    x,
    y
};

enum axis_scal { linear, logarithmic }; // tbd: date, time

struct axis_ticks // tickmarks of axis
{

    axis_ticks() = default;
    axis_ticks(double mj_anchor, double mj_delta, int mn_intv) :
        major_anchor{mj_anchor}, major_delta{mj_delta}, minor_intervals{mn_intv}
    {
    }
    axis_ticks(const axis_ticks& other) = default;
    axis_ticks& operator=(const axis_ticks& other) = default;
    axis_ticks(axis_ticks&& other) = default;
    axis_ticks& operator=(axis_ticks&& other) = default;

    double major_anchor{0.0}; // anchor for major intervals
                              // i.e. point on axis around witch major
                              // intervals are centered
                              //
                              // INFO: ignored for log scaled axis

    double major_delta{1.0}; // distance between two major notches
                             // each enclosed with major notches (0: none)
                             // major notches will be numbered
                             //
                             // INFO: ignored for log scaled axis

    int minor_intervals{4}; // number of intervals within each major interval
                            // minor notches will not be numbered (0: none)
                            //
                            // INFO: ignored for log scaled axis
};

struct widget_axis_data {

    widget_axis_data() = default;
    widget_axis_data(int widget_size, int axis_woffset, int axis_wlength) :
        w_size{widget_size}, a_offset{axis_woffset}, a_length{axis_wlength}
    {
    }
    widget_axis_data(const widget_axis_data& other) = default;
    widget_axis_data& operator=(const widget_axis_data& other) = default;
    widget_axis_data(widget_axis_data&& other) = default;
    widget_axis_data& operator=(widget_axis_data&& other) = default;

    int w_size{600};
    int a_offset{50};
    int a_length{520};
};

struct axis_data {

    axis_data() = default;
    axis_data(axis_rng a_rng, axis_dir a_dir, axis_scal a_scal, std::string a_label,
              axis_ticks a_ticks) :
        rng{a_rng},
        dir{a_dir}, scal{a_scal}, label{a_label}, ticks{a_ticks}
    {
    }
    axis_data(const axis_data& other) = default;
    axis_data& operator=(const axis_data& other) = default;
    axis_data(axis_data&& other) = default;
    axis_data& operator=(axis_data&& other) = default;

    axis_rng rng;
    // rng must contain scaled limits, i.e.
    // linear: min, max
    // logarithmic: log10(min), log10(max)

    axis_dir dir{axis_dir::x};
    axis_scal scal{axis_scal::linear};

    std::string label{""};

    axis_ticks ticks;
};

class Axis // defines axis and scaling transformation to paint device
           // coordinates layout see Stroustrup, "Programming, Principles and
           // Practice using C++", p. 530ff (setup on p. 542)

// axis values are always to be regarded as scaled values
// (e.g. on logarithmic axis)
// unscaled means original values without axis scaling
{
  public:

    Axis(widget_axis_data wd_in, axis_data ad_in);

    int a_to_w(double scaled_value) const;    // (scaled) axis to widget transformation
    int au_to_w(double unscaled_value) const; // unscaled axis to widget transformation
    double w_to_a(int npos) const;            // widget to (scaled) axis transformation
    double w_to_au(int npos) const;           // widget to unscaled axis transformation

    void draw(QPainter* qp, int offset);

    double min() const { return ad.rng.min; } // min as scaled value
    double max() const { return ad.rng.max; } // max as scaled value
    double major_delta() const { return ad.ticks.major_delta; }
    int nmin() const { return a_to_w(ad.rng.min); }
    int nmax() const { return a_to_w(ad.rng.max); }
    int widget_size() const { return wd.w_size; }
    axis_scal scaling() const { return ad.scal; }
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
    coordsys_data() = default;
    coordsys_data(std::string title_in) : title{title_in} {}
    coordsys_data(const coordsys_data& other) = default;
    coordsys_data& operator=(const coordsys_data& other) = default;
    coordsys_data(coordsys_data&& other) = default;
    coordsys_data& operator=(coordsys_data&& other) = default;

    std::string title{""}; // Coordsys title

    // helper for scrollwheel scaling
    double x_rng_major_delta_target_ratio{};
    double y_rng_major_delta_target_ratio{};
};

class Coordsys {
  public:

    Coordsys(Axis x_in, Axis y_in, coordsys_data cd_in);
    void draw(QPainter* qp);

    coordsys_data get_coordsys_data() const { return cd; }
    double get_xtarget_ratio() const { return cd.x_rng_major_delta_target_ratio; }
    double get_ytarget_ratio() const { return cd.y_rng_major_delta_target_ratio; }

    void adjust_to_resized_widget(int new_w_width, int new_w_height);
    void adjust_to_pan(double dx, double dy);
    void adjust_to_zoom(double new_xmin, double new_xmax, double new_ymin,
                        double new_ymax);
    void adjust_to_wheel_zoom(double new_xmin, double new_xmax, double new_ymin,
                              double new_ymax, double xtarget_ratio,
                              double ytarget_ratio);

    Axis x;
    Axis y;

  protected:

    double get_new_delta(double min, double max, double delta, double new_min,
                         double new_max);
    double get_new_delta_wheel_zoom(double new_min, double new_max, double delta,
                                    double target_ratio);

  private:

    coordsys_data cd;

    // title as qt-String
    QString title;
};