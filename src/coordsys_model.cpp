#include "coordsys_model.hpp"

void Coordsys_model::draw(QPainter* qp, Coordsys* cs)
{

    qp->save();

    { // draw vectors:

        // draw each poly line
        for (int i = 0; i < vec.size(); ++i)
        {
            if (vec_id[i].active)
            { // only draw active vectors into cs

                qp->setPen(vec_mark[i].pen);

                int nx1 = cs->x.au_to_w(vec[i].from.x);
                int ny1 = cs->y.au_to_w(vec[i].from.y);
                int nx2 = cs->x.au_to_w(vec[i].to.x);
                int ny2 = cs->y.au_to_w(vec[i].to.y);
                qp->drawLine(nx1, ny1, nx2, ny2);
            }
        }
    }

    { // draw lines:

        // draw each poly line
        for (int i = 0; i < line.size(); ++i)
        {
            if (line_id[i].active)
            { // only draw active lines into cs

                qp->setPen(line_mark[i].pen);

                // connect all points on each line
                for (int j = 0; j < line[i].size() - 1; ++j)
                {
                    int nx1 = cs->x.au_to_w(line[i][j].x);
                    int ny1 = cs->y.au_to_w(line[i][j].y);
                    int nx2 = cs->x.au_to_w(line[i][j + 1].x);
                    int ny2 = cs->y.au_to_w(line[i][j + 1].y);
                    qp->drawLine(nx1, ny1, nx2, ny2);
                }
            }
        }
    }

    { // draw pts (add other stuff above to make pt_mark in pts appear on top):

        for (int i = 0; i < pt.size(); ++i)
        {
            if (pt_id[i].active)
            { // only draw active pts into cs
                int nx = cs->x.au_to_w(pt[i].x);
                int ny = cs->y.au_to_w(pt[i].y);
                qp->setPen(pt_mark[i].pen);

                switch (pt_mark[i].symbol)
                {
                case Symbol::plus:
                {
                    qp->drawLine(nx - pt_mark[i].nsize, ny, nx + pt_mark[i].nsize,
                                 ny);
                    qp->drawLine(nx, ny - pt_mark[i].nsize, nx,
                                 ny + pt_mark[i].nsize);
                    break;
                }
                case Symbol::cross:
                {
                    qp->drawLine(nx - pt_mark[i].nsize, ny - pt_mark[i].nsize,
                                 nx + pt_mark[i].nsize, ny + pt_mark[i].nsize);
                    qp->drawLine(nx - pt_mark[i].nsize, ny + pt_mark[i].nsize,
                                 nx + pt_mark[i].nsize, ny - pt_mark[i].nsize);
                    break;
                }
                case Symbol::circle:
                {
                    qp->drawEllipse(QPoint(nx, ny), pt_mark[i].nsize,
                                    pt_mark[i].nsize);
                    break;
                }
                case Symbol::square:
                {
                    qp->drawLine(nx - pt_mark[i].nsize, ny - pt_mark[i].nsize,
                                 nx + pt_mark[i].nsize, ny - pt_mark[i].nsize);
                    qp->drawLine(nx + pt_mark[i].nsize, ny - pt_mark[i].nsize,
                                 nx + pt_mark[i].nsize, ny + pt_mark[i].nsize);
                    qp->drawLine(nx + pt_mark[i].nsize, ny + pt_mark[i].nsize,
                                 nx - pt_mark[i].nsize, ny + pt_mark[i].nsize);
                    qp->drawLine(nx - pt_mark[i].nsize, ny + pt_mark[i].nsize,
                                 nx - pt_mark[i].nsize, ny - pt_mark[i].nsize);
                    break;
                }
                }
            }
        }
    }

    qp->restore();
}

[[maybe_unused]] int Coordsys_model::add_p(const pt2d& p_in,
                                           const pt2d_mark m)
{

    pt.push_back(p_in);
    pt_mark.push_back(m);

    mark_id new_id;
    new_id.id = unique_id++;
    pt_id.push_back(new_id);

    return new_id.id;
}

//
// hint: using ln2d = std::vector<pt2d>;
//
[[maybe_unused]] int Coordsys_model::add_l(const ln2d& vp_in,
                                           const ln2d_mark m)
{

    // the separate copy should not be needed, since it is done in push_back
    // anyway
    //
    // std::vector<pt2d> v;
    // std::copy(vp_in.begin(), vp_in.end(), std::back_inserter(v));
    //
    line.push_back(vp_in);
    line_mark.push_back(m);

    mark_id new_id;
    new_id.id = unique_id++;
    line_id.push_back(new_id);

    if (m.mark_pts == true)
    { // add points of line to pts marked in model

        for (int i = 0; i < vp_in.size(); i += m.delta)
        {

            pt.push_back(vp_in[i]);
            pt_mark.push_back(m.pm);

            mark_id new_pt_id;
            new_pt_id.id = unique_id++;
            new_pt_id.linked_to_id = new_id.id;
            pt_id.push_back(new_pt_id);
        }
    }

    return new_id.id;
}

[[maybe_unused]] int Coordsys_model::add_v(const vec2d& v_in,
                                           const vec2d_mark m)
{

    vec.push_back(v_in);
    vec_mark.push_back(m);

    mark_id new_id;
    new_id.id = unique_id++;
    vec_id.push_back(new_id);

    return new_id.id;
}

void Coordsys_model::set_label(const std::string& new_label)
{

    m_label = new_label;
}

void Coordsys_model::clear()
{
    unique_id = 0;

    pt.clear();
    pt_mark.clear();
    pt_id.clear();

    line.clear();
    line_mark.clear();
    line_id.clear();

    m_label.clear();
}