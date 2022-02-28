#include "w_statusbar.hpp"

#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>

#include <cmath> // for axis scaling (and mathematical functions)

w_Statusbar::w_Statusbar(int width, QWidget* parent) :
    QWidget(parent), w_width(width)
{

    setMinimumSize(w_width, w_height);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    updateGeometry();
}

void w_Statusbar::resizeEvent(QResizeEvent* event)
{
    QSize currentSize = event->size();
    // only update changed width since heigth is kept constant
    if (currentSize.width() != w_width)
    {
        w_width = currentSize.width();
    }
}

void w_Statusbar::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter qp(this);
    draw(&qp);
}

void w_Statusbar::draw(QPainter* qp)
{

    qp->save();

    const int nypos = 14;       // ypos of all displayed strings
    const int border_dist = 10; // minimum distance from left and right border

    // define logical coordinates (start with (0,0) in upper left corner)
    qp->setWindow(QRect(0, 0, w_width, w_height));

    qp->setBrush(QBrush(QColor(Qt::lightGray)));
    qp->setPen(QPen(Qt::lightGray, 1, Qt::SolidLine));
    qp->drawRect(0, 0, w_width, w_height);

    qp->setFont(QFont("Helvetica", 12, QFont::Normal));
    qp->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    QFontMetrics fm = qp->fontMetrics();

    // print number of avialable undo steps
    QString u = QString("#Undo: ") + QString::number(m_undo_steps);
    int undo_len = fm.horizontalAdvance(u);
    qp->drawText(border_dist, nypos, u);

    // print current pan and zoom modes
    QString m;
    switch (m_mode)
    {
    case pz_mode::x_and_y:

        switch (m_action)
        {
        case pz_action::none:
            m = QString("Mode: X & Y");
            break;
        case pz_action::pan:
            m = QString("Pan: X & Y");
            break;
        case pz_action::zoom:
            m = QString("Zoom: X & Y");
            break;
        case pz_action::wheel_zoom:
            m = QString("Wheel Zoom: X & Y");
            break;
        }
        break;

    case pz_mode::x_only:

        switch (m_action)
        {
        case pz_action::none:
            m = QString("Mode: X");
            break;
        case pz_action::pan:
            m = QString("Pan: X");
            break;
        case pz_action::zoom:
            m = QString("Zoom: X");
            break;
        case pz_action::wheel_zoom:
            m = QString("Wheel Zoom: X");
            break;
        }
        break;

    case pz_mode::y_only:
        switch (m_action)
        {
        case pz_action::none:
            m = QString("Mode: Y");
            break;
        case pz_action::pan:
            m = QString("Pan: Y");
            break;
        case pz_action::zoom:
            m = QString("Zoom: Y");
            break;
        case pz_action::wheel_zoom:
            m = QString("Wheel Zoom: Y");
            break;
        }
        break;
    }
    qp->drawText(border_dist + undo_len + 15, nypos, m);

    // print (x,y)-Position of mouse pointer in hot area
    // data must be send as scaled data for logarithmic scaling
    if (m_hot)
    {
        QString x = QString::number(m_x, 'g', 3);
        QString y = QString::number(m_y, 'g', 3);

        QString s1;
        switch (m_xscaling)
        {
        case axis_scal::linear:
            s1 = QString("(x = ") + x;
            break;
        case axis_scal::logarithmic:
            s1 = QString("(log10(x) = ") + x;
            break;
        }
        QString s2;
        switch (m_yscaling)
        {
        case axis_scal::linear:
            s2 = QString(", y = ") + y + QString(")");
            break;
        case axis_scal::logarithmic:
            s2 = QString(", log10(y) = ") + y + QString(")");
            break;
        }

        QString s = s1 + s2;
        qp->drawText(w_width / 2 - fm.horizontalAdvance(s) / 2, nypos, s);
    }

    // print index and (if present) label of currently displayed model
    QString step = QString("Model: ") + QString::number(m_step);
    if (m_label != "")
    {
        step += QString("  Label: ") + m_label.c_str();
    }
    qp->drawText(w_width - fm.horizontalAdvance(step) - border_dist, nypos, step);

    qp->restore();
}

void w_Statusbar::on_mouseMoved(bool hot, double x, double y)
{

    if (m_x != x || m_y != y || m_hot != hot)
    {
        // update only if any value has changed
        // fmt::print("received event: {} {} {}\n", hot, x, y);
        m_hot = hot;
        m_x = x;
        m_y = y;
        update();
    }
}

void w_Statusbar::on_modelChanged(int step)
{

    if (m_step != step)
    {
        // update only if any value has changed
        // fmt::print("received modelChanged event: {}\n", step);
        m_step = step;
        update();
    }
}

void w_Statusbar::on_modeChanged(pz_action action, pz_mode mode)
{

    if (m_action != action || m_mode != mode)
    {
        // update only if any value has changed
        // fmt::print("received modeChanged event.\n");
        m_action = action;
        m_mode = mode;
        update();
    }
}

void w_Statusbar::on_undoChanged(int undo_steps)
{

    if (m_undo_steps != undo_steps)
    {
        // update only if any value has changed
        // fmt::print("received undoChanged event: {}\n", undo_steps);
        m_undo_steps = undo_steps;
        update();
    }
}

void w_Statusbar::on_labelChanged(std::string label)
{

    if (m_label != label)
    {
        // update only if any value has changed
        // fmt::print("received labelChanged event: {}\n", label);
        m_label = label;
        update();
    }
}

void w_Statusbar::on_scalingChanged(axis_scal xscal, axis_scal yscal)
{

    if (m_xscaling != xscal || m_yscaling != yscal)
    {
        // update only if any value has changed
        // fmt::print("received scalingChanged event.\n");
        m_xscaling = xscal;
        m_yscaling = yscal;
        update();
    }
}