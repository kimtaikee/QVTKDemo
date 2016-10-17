#include "widget.h"

#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>


VTKWidget::VTKWidget(QWidget *parent) : QVTKWidget(parent)
{
    vtkObject::GlobalWarningDisplayOff();

    GetRenderWindow()->SetMultiSamples(0);

    vtkRenderer* renderer = vtkRenderer::New();

    vtkActor* actor = vtkActor::New();
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    vtkSphereSource* sphereSrc = vtkSphereSource::New();
    sphereSrc->Update();

    mapper->SetInputData(sphereSrc->GetOutput());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);

    GetRenderWindow()->AddRenderer(renderer);

    setWindowTitle(tr("Render Sphere"));
}
