#ifndef POINTSTOPOLYDATACONVERTER_H
#define POINTSTOPOLYDATACONVERTER_H

#include <utils/point3f.h>
#include <utils/signalledrunable.h>

#include <vtkSmartPointer.h>

#include "vtkutils_global.h"

class vtkPolyData;
namespace VtkUtils
{
class VTKUTILS_EXPORT PointsToPolyDataConverter : public Utils::SignalledRunnable
{
    Q_OBJECT
public:
    explicit PointsToPolyDataConverter(const QList<Utils::Point3F>& points);

    void run();

    vtkPolyData* polyData() const;

private:
    QList<Utils::Point3F> m_points;
    vtkSmartPointer<vtkPolyData> m_polyData;
};

} // namespace VtkUtils
#endif // POINTSTOPOLYDATACONVERTER_H
