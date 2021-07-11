#pragma once

#include "coordsys.hpp"
#include "coordsys_model.hpp"

#include <QPainter>
#include <QWidget>
#include <QtWidgets>

// pan and zoom action
enum class pz_action { none, pan, zoom, wheel_zoom };

// mode restriction for pan and zoom handling
// x_and_y: no restriction
// x_only:  restrict pan/zoom to x axis
// y_only:  restrict pan/zoom to y axis
enum class pz_mode { x_and_y, x_only, y_only };

class w_Coordsys : public QWidget {
  Q_OBJECT

public:
  w_Coordsys(Coordsys* cs, Coordsys_model* cm, QWidget* parent = nullptr);
  w_Coordsys(Coordsys* cs, const std::vector<Coordsys_model*> vm,
             QWidget* parent = nullptr);

  // ATTENTION: caller responsible that model ptr vm is valid during life time

protected:
  void resizeEvent(QResizeEvent* event);
  void paintEvent(QPaintEvent* event);
  void draw(QPainter* qp);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);

  void push_to_history();  // for undo
  void pop_from_history(); // undo

private slots:
  void switch_to_model(int);

signals:
  void mouseMoved(bool hot, double x, double y);
  void modeChanged(pz_action action, pz_mode mode);
  void undoChanged(int undo_steps);
  void labelChanged(std::string new_label);

private:
  Coordsys* cs;
  Coordsys_model* cm;               // current modell
  std::vector<Coordsys_model*> vm;  // vector of models (owned externally)
                                    // that might be switched between
                                    // in case of several models
  std::vector<Coordsys> cs_history; // history of coordinate-systems (for undo)

  // mouse status
  int m_nx{0};                         // x-position of mouse in widget
  int m_ny{0};                         // y-position of mouse in widget
  int m_nx_hot{0};                     // x-position of mouse in hot area
  int m_ny_hot{0};                     // y-position of mouse in hot area
  bool m_hot{false};                   // mouse within coordsys area
  bool m_leftButton{false};            // left button pressed in hot area
  bool m_rightButton{false};           // right button pressed in hot area
  pz_action m_action{pz_action::none}; // normal movement, no pan or zoom
  pz_mode m_mode{pz_mode::x_and_y};    // no pan or zoom restrictions
  int m_nx_leftPress{0};               // x-position of leftButtonPress-Event
  int m_ny_leftPress{0};               // y-position of leftButtonPress-Event
  double m_ref_xmin, m_ref_xmax;       // reference xmin, xmax for wheel_zoom
  double m_ref_ymin, m_ref_ymax;       // reference ymin, ymax for wheel_zoom
  double m_ref_xdelta, m_ref_ydelta;   // reference major_delta for wheel_zoom
};
