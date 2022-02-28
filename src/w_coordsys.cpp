#include "w_coordsys.hpp"
#include <QCursor>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QString>
#include <QWheelEvent>

#include <algorithm> // for std::min and std::max
#include <cmath>     // for axis scaling (and mathematical functions)

w_Coordsys::w_Coordsys(Coordsys* cs, Coordsys_model* cm, QWidget* parent) :
    QWidget(parent), cs(cs), cm(cm)
{

    // store current model ptr in list of models (in case other models are added
    // later)
    vm.push_back(cm);

    setMinimumSize(cs->x.widget_size(), cs->y.widget_size());
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    updateGeometry();

    // receive Mouse Move Events even when no button is pressed (default is false)
    // required to inform the status bar about the current mouse position
    setMouseTracking(true);
    // Accept KeyPress and KeyRelease Events
    setFocusPolicy(Qt::StrongFocus);
}

w_Coordsys::w_Coordsys(Coordsys* cs, const std::vector<Coordsys_model*> vm,
                       QWidget* parent) :
    QWidget(parent),
    cs(cs), vm(vm)
{

    // set current model ptr to first entry
    cm = vm[0];

    setMinimumSize(cs->x.widget_size(), cs->y.widget_size());
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    updateGeometry();

    // receive Mouse Move Events even when no button is pressed (default is false)
    // required to inform the status bar about the current mouse position
    setMouseTracking(true);
    // Accept KeyPress and KeyRelease Events
    setFocusPolicy(Qt::StrongFocus);
}

void w_Coordsys::resizeEvent(QResizeEvent* event)
{
    QSize oldSize = event->oldSize();
    QSize currentSize = event->size(); // new widget size after resize
    if (oldSize != currentSize)
    {
        cs->adjust_to_resized_widget(currentSize.width(), currentSize.height());
    }
}

void w_Coordsys::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter qp(this);
    qp.setRenderHint(QPainter::Antialiasing);
    draw(&qp);
}

void w_Coordsys::draw(QPainter* qp)
{

    cs->draw(qp);
    cm->draw(qp, cs);

    // fmt::print("w_Coorsys::draw\n");

    if (m_leftButton)
    {

        qp->save();
        qp->setPen(QPen(Qt::blue, 2, Qt::SolidLine));
        qp->setBrush(QColor(240, 230, 50, 128)); // transparent yellow

        switch (m_mode)
        {
        case pz_mode::x_and_y:
            // draw zoom rectangle

            qp->drawRect(m_nx_leftPress, m_ny_leftPress, m_nx_hot - m_nx_leftPress,
                         m_ny_hot - m_ny_leftPress);

            break;
        case pz_mode::x_only:
            qp->drawRect(m_nx_leftPress, cs->y.nmax(), m_nx_hot - m_nx_leftPress,
                         cs->y.nmin() - cs->y.nmax());
            break;
        case pz_mode::y_only:
            qp->drawRect(cs->x.nmin(), m_ny_leftPress, cs->x.nmax() - cs->x.nmin(),
                         m_ny_hot - m_ny_leftPress);
            break;
        }
        qp->restore();
    }
}

void w_Coordsys::keyPressEvent(QKeyEvent* event)
{

    // ignore key repetition, just change the mode if required
    if (event->key() == Qt::Key_X && m_mode != pz_mode::x_only)
    {
        m_mode = pz_mode::x_only;
        // fmt::print("X pressed\n");
        emit modeChanged(m_action, m_mode);
    }
    if (event->key() == Qt::Key_Y && m_mode != pz_mode::y_only)
    {
        m_mode = pz_mode::y_only;
        // fmt::print("Y pressed\n");
        emit modeChanged(m_action, m_mode);
    }
    if (event->key() == Qt::Key_Z && (event->modifiers() & Qt::ControlModifier))
    {
        // call undo function to reinstate last coordsys
        pop_from_history();
    }
}

void w_Coordsys::keyReleaseEvent(QKeyEvent* event)
{

    if (event->key() == Qt::Key_X)
    {
        m_mode = pz_mode::x_and_y;
        // fmt::print("X released\n");
        emit modeChanged(m_action, m_mode);
    }
    if (event->key() == Qt::Key_Y)
    {
        m_mode = pz_mode::x_and_y;
        // fmt::print("Y released\n");
        emit modeChanged(m_action, m_mode);
    }
}

void w_Coordsys::mousePressEvent(QMouseEvent* event)
{

    // accept mouse presses only in hot area
    if (m_hot)
    {
        if (event->button() == Qt::LeftButton)
        {
            // fmt::print("w_Coordsys::mousePressEvent() left button (zoom)\n");

            // zoom
            m_leftButton = true;
            m_action = pz_action::zoom;
            m_nx_leftPress = event->pos().x();
            m_ny_leftPress = event->pos().y();
            emit modeChanged(m_action, m_mode);
        }

        if (event->button() == Qt::RightButton)
        {
            // fmt::print("w_Coordsys::mousePressEvent() right button (pan)\n");

            // pan
            m_rightButton = true;
            m_action = pz_action::pan;
            setCursor(QCursor(Qt::OpenHandCursor));

            // store undo info (before pan starts, don't store intermediate steps)
            // call update to statusbar only on keyRelease, to not confuse the user
            push_to_history();
            emit modeChanged(m_action, m_mode);
        }
    }
}

void w_Coordsys::mouseReleaseEvent(QMouseEvent* event)
{

    // end of zoom event triggered by release of left mouse button
    if (event->button() == Qt::LeftButton && m_leftButton)
    {
        // fmt::print("w_Coordsys::mouseReleaseEvent() left button\n");

        // reset action, if active action was zoom
        if (m_action == pz_action::zoom)
        {
            m_leftButton = false;
            m_action = pz_action::none;
            emit modeChanged(m_action, m_mode);
        }

        // return scaled values as new min and max
        double new_xmin =
            std::min(cs->x.w_to_a(m_nx_leftPress), cs->x.w_to_a(m_nx_hot));
        double new_xmax =
            std::max(cs->x.w_to_a(m_nx_leftPress), cs->x.w_to_a(m_nx_hot));
        double new_ymin =
            std::min(cs->y.w_to_a(m_ny_leftPress), cs->y.w_to_a(m_ny_hot));
        double new_ymax =
            std::max(cs->y.w_to_a(m_ny_leftPress), cs->y.w_to_a(m_ny_hot));

        // fmt::print("xmin={}, xmax={}, ymin={}, ymax={}\n", new_xmin, new_xmax,
        //            new_ymin, new_ymax);

        // only adjust if remaining new x- and y-axis are larger than zero
        // otherwise ignore zoom request
        if (new_xmin != new_xmax && new_ymin != new_ymax)
        {

            // fmt::print("w_Coordsys::mouseReleaseEvent() left button with change\n\n");

            // store undo info (only if zoom actually is performed)
            // and update statusbar emediately
            push_to_history();
            emit undoChanged(cs_history.size()); // update undo info in status bar

            // adjust and update
            switch (m_mode)
            {
            case pz_mode::x_and_y:
                cs->adjust_to_zoom(new_xmin, new_xmax, new_ymin, new_ymax);
                break;
            case pz_mode::x_only:
                cs->adjust_to_zoom(new_xmin, new_xmax, cs->y.min(), cs->y.max());
                break;
            case pz_mode::y_only:
                cs->adjust_to_zoom(cs->x.min(), cs->x.max(), new_ymin, new_ymax);
                break;
            }
            update();
        }
    }

    // end of pan event triggered by release of right mouse button
    if (event->button() == Qt::RightButton && m_rightButton)
    {
        // fmt::print("w_Coordsys::mouseReleaseEvent() right button\n");

        // reset action, if active action was pan
        if (m_action == pz_action::pan)
        {

            // fmt::print("w_Coordsys::mouseReleaseEvent() end of pan.\n\n");

            // update statusbar (cs_history.size() was changed on keyPressEvent)
            emit undoChanged(cs_history.size());

            m_rightButton = false;
            m_action = pz_action::none;
            emit modeChanged(m_action, m_mode);
        }

        if (m_hot)
        {
            setCursor(QCursor(Qt::CrossCursor));
        }
        else
        {
            setCursor(QCursor());
        }
    }
}

void w_Coordsys::mouseMoveEvent(QMouseEvent* event)
{

    // current mouse position in widget
    int nx = event->pos().x();
    int ny = event->pos().y();

    if (nx != m_nx || ny != m_ny)
    {
        // mouse moved to a new postion

        // convert coordinates to scaled values
        double x_pos = cs->x.w_to_a(nx);
        double y_pos = cs->y.w_to_a(ny);

        // determine if mouse is in active cs area
        bool hot = false;
        if (x_pos >= cs->x.min() && x_pos <= cs->x.max() && y_pos >= cs->y.min() &&
            y_pos <= cs->y.max())
        {
            hot = true;
        }
        m_hot = hot; // store whether mouse is still in hot area

        // send current mouse position to status bar
        emit mouseMoved(hot, x_pos, y_pos);

        // current mouse position in hot area (needed for zoom rectangle)
        if (nx < cs->x.nmin())
        {
            m_nx_hot = cs->x.nmin();
        }
        else if (nx > cs->x.nmax())
        {
            m_nx_hot = cs->x.nmax();
        }
        else
        {
            m_nx_hot = nx;
        }

        // different coordinate growth direction
        if (ny > cs->y.nmin())
        {
            m_ny_hot = cs->y.nmin();
        }
        else if (ny < cs->y.nmax())
        {
            m_ny_hot = cs->y.nmax();
        }
        else
        {
            m_ny_hot = ny;
        }

        // switch to crosshair cursor in hot area if right button is not pressed
        if (m_hot && cursor() != Qt::CrossCursor && m_rightButton == false)
        {
            setCursor(QCursor(Qt::CrossCursor));
        }
        // switch to open hand cursor in hot area if right button is pressed
        if (m_hot && cursor() != Qt::OpenHandCursor && m_rightButton == true)
        {
            setCursor(QCursor(Qt::OpenHandCursor));
        }

        // switch back to default cursor outside of hot area
        if (!m_hot)
        {
            setCursor(QCursor());
        }

        // pan (only in hot area)
        if (m_rightButton && m_hot)
        {
            double dx = x_pos - cs->x.w_to_a(m_nx);
            double dy = y_pos - cs->y.w_to_a(m_ny);
            // fmt::print("dx = {}, dy = {}\n", dx, dy);
            switch (m_mode)
            {
            case pz_mode::x_and_y:
                cs->adjust_to_pan(dx, dy);
                break;
            case pz_mode::x_only:
                cs->adjust_to_pan(dx, 0.0);
                break;
            case pz_mode::y_only:
                cs->adjust_to_pan(0.0, dy);
                break;
            }
            update();
        }

        // store current position
        m_nx = nx;
        m_ny = ny;

        // zoom (initiate update for drawing the zoom frame)
        // allow update outside of hot area as well to include boundaries of hot
        // area easily
        if (m_leftButton)
        {
            update();
        }

        // fmt::print("m_nx = {}, m_ny = {}\n", m_nx, m_ny);
        // fmt::print("nx = {}, ny = {}\n", nx, ny);
        // fmt::print("delta_nx = {}, delta_ny = {}\n", delta_nx, delta_ny);

        // fmt::print("hot = {}, left = {}, right = {}\n", m_hot, m_leftButton,
        //            m_rightButton);

        // fmt::print("nx = {}, ny = {}, x_pos = {}, y_pos = {}\n", nx, ny, x_pos,
        //            y_pos);
    }
}

void w_Coordsys::wheelEvent(QWheelEvent* event)
{

    // one tick corresponds to 1/8°
    // scroll wheel ticks towards user are > 0; ticks away from user are negative
    // on MacOS two finger movement without pressing is also registered as
    // wheelEvent

    if (event->isEndEvent())
    {
        // fmt::print("w_Coordsys::wheelEvent->isEndEvent()\n");

        // reset action, if active action was wheel_zoom
        if (m_action == pz_action::wheel_zoom)
        {
            m_action = pz_action::none;
            emit modeChanged(m_action, m_mode);
        }
        return;
    }

    // just use the y-direction ticks as indication for requested zoom factor
    int numTicks = event->angleDelta().y();

    if (m_hot && (numTicks != 0))
    {

        if (m_action == pz_action::none)
        {
            // for new scrollwheel moves, store undo information

            // fmt::print("w_Coordsys::wheelEvent Begin.\n");

            // store undo info only if wheel zoom actually is started
            // and update statusbar immediately
            m_action = pz_action::wheel_zoom;
            emit modeChanged(m_action, m_mode);

            push_to_history();
            emit undoChanged(cs_history.size()); // update undo info in status bar
        }

        // fmt::print("w_Coordsys::wheelEvent moving.\n");

        // numTicks is used for scaling: positive values for zoom in, negative for
        // zoom out center point for the scaling is the mouse cursor position
        // m_nx_hot, m_ny_hot
        // the distance between the x- and y-values at the current mouse position
        // and the current min- and max-positions are scaled

        // fmt::print(
        //     "Received a wheelEvent at m_nx_hot = {}, m_ny_hot = {}, m_hot = {}, "
        //     "numTicks = {}\n",
        //     m_nx_hot, m_ny_hot, m_hot, numTicks);

        double x = cs->x.w_to_a(m_nx_hot);
        double y = cs->y.w_to_a(m_ny_hot);

        double dx_min = x - cs->x.min();
        double dx_max = cs->x.max() - x;

        double dy_min = y - cs->y.min();
        double dy_max = cs->y.max() - y;

        double scale_fact = 1.0 - 0.01 * 0.25 * numTicks; // 4 numTicks = 1% scaling

        double new_xmin = x - scale_fact * dx_min;
        double new_xmax = x + scale_fact * dx_max;

        double new_ymin = y - scale_fact * dy_min;
        double new_ymax = y + scale_fact * dy_max;

        // fmt::print("new_xmin={}, new_xmax={}, new_ymin={}, new_ymax={}\n\n",
        //            new_xmin, new_xmax, new_ymin, new_ymax);

        // fmt::print("w_coordsys::wheelEvent: xtarget_ratio = {}, ytarget_ratio = {}\n\n",
        //            cs->get_xtarget_ratio(), cs->get_ytarget_ratio());

        // adjust and update
        switch (m_mode)
        {
        case pz_mode::x_and_y:
            cs->adjust_to_wheel_zoom(new_xmin, new_xmax,
                                     new_ymin, new_ymax,
                                     cs->get_xtarget_ratio(),
                                     cs->get_ytarget_ratio());
            break;
        case pz_mode::x_only:
            cs->adjust_to_wheel_zoom(new_xmin, new_xmax,
                                     cs->y.min(), cs->y.max(),
                                     cs->get_xtarget_ratio(),
                                     cs->get_ytarget_ratio());
            break;
        case pz_mode::y_only:
            cs->adjust_to_wheel_zoom(cs->x.min(), cs->x.max(),
                                     new_ymin, new_ymax,
                                     cs->get_xtarget_ratio(),
                                     cs->get_ytarget_ratio());
            break;
        }
        update();
    }
}

void w_Coordsys::push_to_history()
{

    // push a copy of the current cs into cs_history
    cs_history.emplace_back(Coordsys(cs->x, cs->y, cs->get_coordsys_data()));
}

void w_Coordsys::pop_from_history()
{

    // undo function to restore older version of cs
    if (cs_history.size() > 0)
    {
        // fmt::print("got undo signal: cs_history.size()={}\n",
        // cs_history.size());
        *cs = cs_history[cs_history.size() - 1];
        cs_history.pop_back();
        // just in case the widet was resized => adjust to current size
        cs->adjust_to_resized_widget(width(), height());
        emit undoChanged(cs_history.size()); // update undo info in status bar
        update();
    }
}

void w_Coordsys::switch_to_model(int idx)
{

    if (idx >= 0 && idx < vm.size())
    {
        // fmt::print("got signal {}\n", idx);
        cm = vm[idx];
        emit labelChanged(cm->label());
        update();
    }
}
