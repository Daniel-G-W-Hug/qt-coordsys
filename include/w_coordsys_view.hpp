#pragma once

#include "coordsys.hpp"
#include "coordsys_model.hpp"

#include <QPainter>
#include <QWidget>

class w_Coordsys_view : public QWidget {
  Q_OBJECT

public:
  w_Coordsys_view(Coordsys *cs, Coordsys_model *cm, QWidget *parent = 0);

protected:
  void paintEvent(QPaintEvent *event);
  void draw(QPainter *qp);

private:
  Coordsys *cs;
  Coordsys_model *cm;
};
