#pragma once

#include <QGraphicsItem>

class w_Coordsys;

class active_pt2d : public QGraphicsItem {
  public:

    active_pt2d(w_Coordsys* wcs);

    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

  protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  private:

    QPointF newPos;
    w_Coordsys* wcs;
};
