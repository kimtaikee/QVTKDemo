#include "arrowitem.h"

#include <math.h>

#include <QPen>
#include <QPainter>

const qreal Pi = 3.14;
static const int ArrowSize = 10;

ArrowItem::ArrowItem(const QPointF& startPot, const QPointF& endPot, QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
{
    m_startPoint = startPot;
    m_endPoint = endPot;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF ArrowItem::boundingRect() const
{
    qreal extra = (pen().width() + ArrowSize);

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath ArrowItem::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

void ArrowItem::updatePosition()
{
    QLineF line(m_startPoint, m_endPoint);
    setLine(line);
}

void ArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    QPen myPen = pen();
    myPen.setColor(myColor);
    myPen.setWidth(1);
    qreal arrowSize = ArrowSize;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    setLine(QLineF(m_startPoint, m_endPoint));

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                        cos(angle + Pi / 3) * arrowSize);
        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                        cos(angle + Pi - Pi / 3) * arrowSize);

        arrowHead.clear();
        arrowHead << line().p1() << arrowP1 << arrowP2;
        painter->drawLine(line());
        painter->drawPolygon(arrowHead);
        if (isSelected()) {
            painter->setPen(QPen(myColor, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}
