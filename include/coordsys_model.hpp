#pragma once

#include "coordsys.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <cassert> // attribute [[maybe_unused]]
#include <vector>

#include "fmt/format.h"
#include "fmt/ranges.h" // support printing of (nested) containers & tuples

using namespace fmt::literals; // just to make the format literals visible

enum Symbol
{
    plus,
    cross,
    circle,
    square
};

struct pt2d
{ // coordinates of point on x and y axis
    double x{0.0}, y{0.0};
    pt2d(double x_in, double y_in) :
        x(x_in), y(y_in) {}
    pt2d() = default;
};

// this struct should be used by the user to mark points
struct pt2d_mark
{

    Symbol symbol{plus}; // define the marking symbol
    int nsize{3};        // characteristic size of mark symbol in pixels
    QPen pen{QPen(Qt::blue, 2, Qt::SolidLine)};

    int grp{0}; // user provided group the
                // item shall belong to (for selection)
};

const pt2d_mark pt2d_mark_default; // for default arguments

// this struct should be used by the user to mark lines
struct ln2d_mark
{

    QPen pen{QPen(Qt::black, 1, Qt::SolidLine)};

    bool mark_pts{false};
    int delta{1}; // 1 shows every point, 2 every second, ...
    pt2d_mark pm{};

    bool mark_area{false};
    QColor area_col{QColor(0, 128, 0, 128)};

    int grp{0}; // user provided group the
                // item shall belong to (for selection)
};

const ln2d_mark ln2d_mark_default; // for default arguments;

// vector as directed line
struct vec2d
{
    pt2d from; // from = (0,0) => position vector
               // else => free vector
    pt2d to;
};

// this struct should be used by the user to mark vectors
struct vec2d_mark
{

    QPen pen{QPen(Qt::black, 1, Qt::SolidLine)};

    int grp{0}; // user provided group the
                // item shall belong to (for selection)
};

const vec2d_mark vec2d_mark_default; // for default arguments;

// ----------------------------------------------------------------------------
// this is used internally, not by the user directly
// ----------------------------------------------------------------------------

struct mark_id
{

    // system provided unique_id to identify each item in the model
    int id{-1};           // valid unique ids are positive values
    int linked_to_id{-1}; // associated with other id (e.g. pt_marks with line)
    bool active{true};    // active elements are diplayed (on by default)
};

// ----------------------------------------------------------------------------
// this is used internally up to here, not by the user directly
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// convenience alias to make pt2d and ln2d look similar
// ----------------------------------------------------------------------------
using ln2d = std::vector<pt2d>;
// ----------------------------------------------------------------------------

class Coordsys_model
{
  public:

    void draw(QPainter* qp, Coordsys* cs);

    // add single point
    [[maybe_unused]] int add_p(const pt2d& p_in,
                               const pt2d_mark m = pt2d_mark_default);
    // add single line
    [[maybe_unused]] int add_l(const std::vector<pt2d>& vp_in,
                               const ln2d_mark m = ln2d_mark_default);
    // add vector
    [[maybe_unused]] int add_v(const vec2d& v_in,
                               const vec2d_mark m = vec2d_mark_default);

    void set_label(const std::string& new_label);
    std::string label() { return m_label; }

    // reset model to empty state, e.g. for reuse in new model
    void clear();

  private:

    int unique_id{0}; // id = unique id, e.g. to identify each item in model
                      // assigned when model is setup using push_back calls

    // data for points (same index is for same point)
    std::vector<pt2d> pt;
    std::vector<pt2d_mark> pt_mark;
    std::vector<mark_id> pt_id;

    // data for lines (same index is for same line)
    std::vector<ln2d> line;
    std::vector<ln2d_mark> line_mark;
    std::vector<mark_id> line_id;

    // data for vectors (same index is for same vector)
    std::vector<vec2d> vec;
    std::vector<vec2d_mark> vec_mark;
    std::vector<mark_id> vec_id;

    // model label (e.g. time stamp description)
    std::string m_label;
};

// ----------------------------------------------------------------------------
// printing support via fmt library
// ----------------------------------------------------------------------------

// formating for user defined types (pt2d)
template <>
struct fmt::formatter<pt2d>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx);
    template <typename FormatContext>
    auto format(const pt2d& pt, FormatContext& ctx);
};

template <typename ParseContext>
constexpr auto fmt::formatter<pt2d>::parse(ParseContext& ctx)
{
    return ctx.begin();
}

template <typename FormatContext>
auto fmt::formatter<pt2d>::format(const pt2d& pt, FormatContext& ctx)
{
    // return fmt::format_to(ctx.out(), "{{{0}, {1}}}", pt.x, pt.y);
    return fmt::format_to(ctx.out(), "({0}, {1})", pt.x, pt.y);
}

// Bsp. für Anwendung
// std::vector<pt2d> vp1{{1.0, 1.0}, {1.5, 2.0}};
// pt2d p{1.0, 2.0};
// fmt::print(" p = {}\n", p);

// fmt::print(" vp1 = {}\n", fmt::join(vp1, ", "));