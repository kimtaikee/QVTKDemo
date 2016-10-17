#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QGraphicsLineItem>

class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;

class ArrowItem : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    ArrowItem(const QPointF& startPot, const QPointF& endPot, QGraphicsItem *parent = 0);

    int type() const Q_DECL_OVERRIDE { return Type; }
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void setColor(const QColor &color) { myColor = color; }
    void setStartPoint(const QPointF& p) { m_startPoint = p; }
    QPointF startPoint() const { return m_startPoint; }
    void setEndPoint(const QPointF& p) { m_endPoint = p; }
    QPointF endPoint() const { return m_endPoint; }

    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) Q_DECL_OVERRIDE;

private:
    QPointF m_startPoint;
    QPointF m_endPoint;
    QColor myColor;
    QPolygonF arrowHead;
};

#endif // ARROW_H
