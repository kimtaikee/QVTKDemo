#include "vtkutils.h"
#include "pointstopolydataconverter.h"

#include <vtkPoints.h>
#include <vtkDoubleArray.h>

namespace VtkUtils
{

PointsToPolyDataConverter::PointsToPolyDataConverter(const QList<Utils::Point3F>& points) : m_points(points)
{

}

void PointsToPolyDataConverter::run()
{
    if (m_points.isEmpty()) {
        emit finished();
        return;
    }

    VtkUtils::vtkInitOnce(m_polyData);

    VTK_CREATE(vtkPoints, vtkpoints);
    VTK_CREATE(vtkDoubleArray, scalarArray);
    scalarArray->SetName("scalar");

    for (int i = 0; i < m_points.size(); ++i) {
        const Utils::Point3F& p3f = m_points.at(i);
        vtkpoints->InsertNextPoint(p3f.x, p3f.y, p3f.z);
        scalarArray->InsertNextTuple1(p3f.z); // z value as scalar
    }

    m_polyData->SetPoints(vtkpoints);
    m_polyData->GetPointData()->SetScalars(scalarArray);

    emit finished();
}

vtkPolyData* PointsToPolyDataConverter::polyData() const
{
    return m_polyData;
}

} // namespace VtkUtils
