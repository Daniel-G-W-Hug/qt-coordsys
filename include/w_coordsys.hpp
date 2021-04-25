#pragma once

#include "coordsys.hpp"
#include "coordsys_model.hpp"

#include <QPainter>
#include <QWidget>
#include <QtWidgets>

class w_Coordsys : public QWidget {
  Q_OBJECT

public:
  w_Coordsys(Coordsys* cs, Coordsys_model* cm, QWidget* parent = nullptr);
  w_Coordsys(Coordsys* cs, const std::vector<Coordsys_model*> vm,
             QWidget* parent = nullptr);

  // ATTENTION: caller is responsible that model ptr is valid during life time

protected:
  void resizeEvent(QResizeEvent* event);
  void paintEvent(QPaintEvent* event);
  void draw(QPainter* qp);
  void mouseMoveEvent(QMouseEvent* event);

private slots:
  void switch_to_model(int);

signals:
  void mouseMoved(bool hot, double x, double y);

private:
  Coordsys* cs;
  Coordsys_model* cm;              // current modell
  std::vector<Coordsys_model*> vm; // vector of models
                                   // that might be switched between
                                   // in case of several models
};
