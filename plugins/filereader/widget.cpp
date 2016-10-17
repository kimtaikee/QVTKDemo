#include "widget.h"

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>

#include <QDebug>

VTKWidget::VTKWidget(QWidget *parent) : QVTKWidget(parent)
{
    vtkObject::GlobalWarningDisplayOff();

    GetRenderWindow()->SetMultiSamples(0);

    vtkRenderer* renderer = vtkRenderer::New();

    vtkActor* actor = vtkActor::New();
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();

    vtkPolyDataReader* reader = vtkPolyDataReader::New();
    reader->SetFileName("E:/vtk/tetraMesh.vtk");
    reader->Update();

    mapper->SetInputData(reader->GetOutput());

    qDebug() << reader->GetOutput()->GetNumberOfPoints();

    actor->SetMapper(mapper);
    renderer->AddActor(actor);

    GetRenderWindow()->AddRenderer(renderer);

    setWindowTitle(tr("QVTK Tutorial: Renderring from vtk file"));
}
