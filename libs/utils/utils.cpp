#include "utils.h"

#include <QProcess>
#include <QPainter>

const float Pi = 3.14159f;

namespace Utils
{

QString character(int index)
{
    char baseChar = 'x';
    QString strChar = QString("%1").arg((char)(baseChar + index % 26));
    return strChar;
}

void explorer(const QString &path)
{
#ifdef Q_OS_WIN
    QString tmpPath(path);
    QStringList params;
    params << "/select," << tmpPath.replace("/", "\\");
    QProcess::startDetached("explorer.exe", params);
#endif
}

QImage star(const QSize& size)
{
    QPainterPath starPath;
    starPath.moveTo(30, 15);
    for (int i = 1; i < 5; ++i) {
        starPath.lineTo(15 + 15 * qCos(0.8 * i * Pi),
                        15 + 15 * qSin(0.8 * i * Pi));
    }
    starPath.closeSubpath();

    QImage star(size, QImage::Format_ARGB32);
    star.fill(Qt::transparent);

    QPainter painter(&star);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QRgb(0xf6a625));
    painter.setBrush(painter.pen().color());
    painter.drawPath(starPath);

    return star;
}

double random(int low, int high)
{
    double f = (double)qrand() / RAND_MAX;
    return f * (high - low) + low;
}

void vtkColor(const QColor& clr, double* vtkClr)
{
    double r = (double)clr.red() / 255;
    double g = (double)clr.green() / 255;
    double b = (double)clr.blue() / 255;
    vtkClr[0] = r;
    vtkClr[1] = g;
    vtkClr[2] = b;
}

QColor qColor(double* pClr)
{
    return QColor(pClr[0] * 255, pClr[1] * 255, pClr[2] * 255);
}

static void RGB2HSV(float r, float g, float b, float *h, float *s, float *v )
{
    float min, max, delta;
    min = qMin(r, qMin(g, b));
    max = qMax(r, qMax(g, b));
    *v = max;				// v
    delta = max - min;
    if(max != 0) {
        *s = delta / max;		// s
    } else {
        // r = g = b = 0		// s = 0, v is undefined
        *s = 0;
        *h = -1;
        return;
    }
    if( r == max )
        *h = ( g - b ) / delta;		// between yellow & magenta
    else if( g == max )
        *h = 2 + ( b - r ) / delta;	// between cyan & yellow
    else
        *h = 4 + ( r - g ) / delta;	// between magenta & cyan
    *h *= 60;				// degrees
    if( *h < 0 )
        *h += 360;
}

void qColor2HSV(const QColor& clr, double* hsv)
{
    double clrArr[3];
    vtkColor(clr, clrArr);
    float h, s, v;
    RGB2HSV(clrArr[0], clrArr[1], clrArr[2], &h, &s, &v);
    hsv[0] = (float) h / 360;
    hsv[1] = s;
    hsv[2] = v;
}

}
