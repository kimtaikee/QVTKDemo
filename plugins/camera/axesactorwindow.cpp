#include "axesactorwindow.h"
#include "ui_axesactorwindow.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkAxesActor.h>
#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkOrientationMarkerWidget.h>

AxesActorWindow::AxesActorWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AxesActorWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Axes Actor Share Camera"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_vtkWidget->GetRenderWindow()->SetNumberOfLayers(2);

    VTK_CREATE(vtkRenderer, actorRenderer);
    VTK_CREATE(vtkArrowSource, arrow);
    VTK_CREATE(vtkPolyDataMapper, actorMapper);
    actorMapper->SetInputConnection(arrow->GetOutputPort());
    vtkActor* actor = vtkActor::New();
    actor->SetMapper(actorMapper);
    actorRenderer->AddActor(actor);
    actorRenderer->Clear();
    m_vtkWidget->GetRenderWindow()->AddRenderer(actorRenderer);
    actorRenderer->SetViewport(0.0, 0.0, 1.0, 1.0);
    actorRenderer->SetLayer(1);
    actorRenderer->SetPreserveColorBuffer(1);

    VTK_CREATE(vtkRenderer, axesRenderer);
    m_vtkWidget->GetRenderWindow()->AddRenderer(axesRenderer);
    VTK_CREATE(vtkAxesActor, axes);
    axes->SetShaftTypeToCylinder();
    axesRenderer->AddActor(axes);
    axesRenderer->SetBackground(1.0, .0, .0);
    axesRenderer->SetViewport(0.0, 0.0, 0.15, 0.15);
    axesRenderer->SetLayer(0);
    axesRenderer->SetActiveCamera(actorRenderer->GetActiveCamera());
    actorRenderer->ResetCamera();
}

AxesActorWindow::~AxesActorWindow()
{
    delete m_ui;
}
