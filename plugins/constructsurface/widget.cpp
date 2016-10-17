#include "widget.h"

#include <utils/tablemodel.h>

#include <utils/tablemodel.h>
#include <utils/utils.h>

#include <vtkSphereSource.h>
#include <vtkDiskSource.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextActor.h>
#include <vtkUnicodeString.h>
#include <vtkTextProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkDelaunay2D.h>

#include <QDebug>

VTKWidget::VTKWidget(QWidget *parent) : QVTKWidget(parent)
{
    vtkObject::GlobalWarningDisplayOff();

    GetRenderWindow()->SetMultiSamples(0);

    vtkRenderer* renderer = vtkRenderer::New();
    renderer->SetBackground(.8, .8, .8);
    m_surfaceActor = vtkActor::New();
    renderer->AddActor(m_surfaceActor);

    GetRenderWindow()->AddRenderer(renderer);
}

void VTKWidget::setModel(Utils::TableModel *model)
{
    // in case multiple connections
    disconnect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    constructSurfaceFromModel(model);
}

void VTKWidget::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Utils::TableModel* model = qobject_cast<Utils::TableModel*>(sender());
    constructSurfaceFromModel(model);
}

void VTKWidget::constructSurfaceFromModel(Utils::TableModel* model)
{
    if (!model)
        return;

    int rows = model->rowCount();

    //
    VTK_CREATE(vtkPoints, points);
    double pot[3];
    for (int r = 0; r < rows; ++r) {
        pot[0] = model->data(r, 0);
        pot[1] = model->data(r, 1);
        pot[2] = model->data(r, 2);

        points->InsertNextPoint(pot);
    }

    VTK_CREATE(vtkPolyData, polyData);
    polyData->SetPoints(points);

    VTK_CREATE(vtkDelaunay2D, del);
    del->SetInputData(polyData);
    del->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(del->GetOutputPort());
    mapper->Update();

    m_surfaceActor->SetMapper(mapper);

    update();
}

