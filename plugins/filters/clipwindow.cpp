#include "clipwindow.h"
#include "ui_generalfilterwindow.h"
#include "ui_cutconfig.h"

#include <utils/utils.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkClipPolyData.h>
#include <vtkClipDataSet.h>
#include <vtkLODActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkBox.h>
#include <vtkSphere.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkLookupTable.h>

#include <QDebug>

ClipWindow::ClipWindow(QWidget *parent) :
    CutWindow(parent)
{
    setWindowTitle(tr("Clip"));
}

ClipWindow::~ClipWindow()
{
}

void ClipWindow::apply()
{
    if (!m_dataObject)
        return;

    if (isValidPolyData()) {

        VTK_CREATE(vtkClipPolyData, clip);
        clip->SetInputData(m_dataObject);

        switch (cutType()) {
        case CutWindow::Plane:
        {
            VTK_CREATE(vtkPlane, plane);
            plane->SetOrigin(m_origin);
            plane->SetNormal(m_normal);
            clip->SetClipFunction(plane);
        }
            break;

        case CutWindow::Box:
        {
            clip->SetClipFunction(m_planes);
        }
            break;

        case CutWindow::Sphere:
        {
            VTK_CREATE(vtkSphere, sphere);
            sphere->SetCenter(m_center);
            sphere->SetRadius(m_radius);
            clip->SetClipFunction(sphere);
        }
            break;
        }

        clip->Update();
        vtkPolyData* pd = vtkPolyData::SafeDownCast(m_dataObject);
        VtkUtils::vtkInitOnce(&m_filterActor);

        VTK_CREATE(vtkPolyDataMapper, mapper);
        mapper->SetInputConnection(clip->GetOutputPort());
        m_filterActor->SetMapper(mapper);
    } else if (isValidDataSet()) {

        VTK_CREATE(vtkClipDataSet, clip);
        clip->SetInputData(m_dataObject);

        switch (cutType()) {
        case CutWindow::Plane:
        {
            VTK_CREATE(vtkPlane, plane);
            plane->SetOrigin(m_origin);
            plane->SetNormal(m_normal);
            clip->SetClipFunction(plane);
        }
            break;

        case CutWindow::Box:
        {
            VTK_CREATE(vtkBox, box);
        }
            break;

        case CutWindow::Sphere:
        {
            VTK_CREATE(vtkSphere, sphere);
            sphere->SetCenter(m_center);
            sphere->SetRadius(m_radius);
            clip->SetClipFunction(sphere);
        }
            break;
        }

        VtkUtils::vtkInitOnce(&m_filterActor);
        setResultData((vtkDataObject*)clip->GetOutput());

        VTK_CREATE(vtkDataSetMapper, mapper);
        mapper->SetInputConnection(clip->GetOutputPort());
        m_filterActor->SetMapper(mapper);
    }

    vtkSmartPointer<vtkLookupTable> lut = createLookupTable(scalarMin(), scalarMax());
    m_filterActor->GetMapper()->SetLookupTable(lut);

    m_vtkWidget->defaultRenderer()->AddActor(m_filterActor);
    m_vtkWidget->update();
    applyDisplayEffect();
}

