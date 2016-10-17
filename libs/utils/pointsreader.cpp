#include "pointsreader.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace Utils
{

PointsReader::PointsReader(const QString& file) : m_file(file)
{

}

void PointsReader::run()
{
    QFile file(m_file);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << QString("PointsReader::run() : cannot open file \"%1\" for reading.").arg(m_file);
        return;
    }

    QTextStream ts(&file);
    QString line;
    QStringList parts;
    while (!ts.atEnd()) {
        line = ts.readLine();
        parts = line.split('\t'); // separated by tab
        parts.removeAll(QString()); // remove empty strings

        if (parts.size() == 3)
            m_points.append(Point3F{parts.at(0).toDouble(), parts.at(1).toDouble(), parts.at(2).toDouble()});
    }

    emit finished();
}

const QList<Point3F>& PointsReader::points() const
{
    return m_points;
}

} // namespace Utils
