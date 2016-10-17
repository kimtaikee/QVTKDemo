#ifndef POINTSTOMODELCONVERTER_H
#define POINTSTOMODELCONVERTER_H

#include <QObject>
#include <QRunnable>

#include "utils.h"
#include "point3f.h"
#include "signalledrunable.h"
#include "utils_global.h"

namespace Utils
{

class TableModel;
class UTILS_EXPORT PointsToModelConverter : public SignalledRunnable
{
    Q_OBJECT
public:
    PointsToModelConverter(const QList<Point3F>& points, TableModel* model);

    void run();

private:
    QList<Point3F> m_points;
    TableModel* m_model = nullptr;
};

} // namespace Utils

#endif // POINTSTOMODELCONVERTER_H
