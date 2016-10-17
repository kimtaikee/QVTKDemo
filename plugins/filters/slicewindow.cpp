#include "slicewindow.h"

#include "ui_generalfilterwindow.h"
#include "ui_cutconfig.h"

#include <utils/utils.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkLODActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkBox.h>
#include <vtkSphere.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkLookupTable.h>

#include <QDebug>

SliceWindow::SliceWindow(QWidget* parent) : CutWindow(parent)
{
    setWindowTitle(tr("Slice"));
}

void SliceWindow::apply()
{
    if (!m_dataObject)
        return;

    VTK_CREATE(vtkCutter, cutter);
    cutter->SetInputData(m_dataObject);

    switch (cutType()) {
    case CutWindow::Plane:
    {
        VTK_CREATE(vtkPlane, plane);
        plane->SetOrigin(m_origin);
        plane->SetNormal(m_normal);
        cutter->SetCutFunction(plane);
    }
        break;

    case CutWindow::Box:
    {
        cutter->SetCutFunction(m_planes);
    }
        break;

    case CutWindow::Sphere:
    {
        VTK_CREATE(vtkSphere, sphere);
        sphere->SetCenter(m_center);
        sphere->SetRadius(m_radius);
        cutter->SetCutFunction(sphere);
    }
        break;
    }

    VtkUtils::vtkInitOnce(&m_filterActor);

    setResultData(cutter->GetOutput());

    vtkSmartPointer<vtkLookupTable> lut = createLookupTable(scalarMin(), scalarMax());

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(cutter->GetOutputPort());
    mapper->SetLookupTable(lut);
    m_filterActor->SetMapper(mapper);

    m_vtkWidget->defaultRenderer()->AddActor(m_filterActor);
    m_vtkWidget->update();

    // update contour lines
    if (m_contourLinesActor && m_contourLinesActor->GetVisibility())
        showContourLines();

    applyDisplayEffect();
}


