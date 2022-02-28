#pragma once

#include "coordsys.hpp"
#include "coordsys_model.hpp"
#include "w_coordsys.hpp"
#include "w_statusbar.hpp"

#include <QPainter>
#include <QWidget>
#include <QtWidgets>

class w_Cs_view : public QWidget
{
    Q_OBJECT

  public:

    w_Cs_view(Coordsys* cs, Coordsys_model* cm, QWidget* parent = nullptr);
    w_Cs_view(Coordsys* cs, const std::vector<Coordsys_model*> vm,
              QWidget* parent = nullptr);

    // ATTENTION: caller is responsible that model ptr is valid during life time

  private:

    w_Coordsys* wcs;
    w_Statusbar* wsb;
    QSlider* slider;

    QGroupBox* w1; // vertical box of CS-widget and slider
};
