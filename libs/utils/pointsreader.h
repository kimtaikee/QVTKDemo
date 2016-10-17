#ifndef POINTSREADER_H
#define POINTSREADER_H

#include <QRunnable>

#include "signalledrunable.h"
#include "utils.h"
#include "point3f.h"
#include "utils_global.h"

namespace Utils
{

class UTILS_EXPORT PointsReader : public SignalledRunnable
{
    Q_OBJECT
public:
    explicit PointsReader(const QString& file);

    void run();

    const QList<Point3F>& points() const;

private:
    QString m_file;
    QList<Point3F> m_points;
};

} // namespace Utils
#endif // POINTSREADER_H
