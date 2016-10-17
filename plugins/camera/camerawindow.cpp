#include "camerawindow.h"
#include "ui_camerawindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>

#include <QDebug>

static void CameraModifiedCallback(vtkObject* caller,
                                   long unsigned int vtkNotUsed(eventId),
                                   void* vtkNotUsed(clientData),
                                   void* vtkNotUsed(callData) )
{
  vtkCamera* camera = static_cast<vtkCamera*>(caller);
  // print the interesting stuff
  qDebug() << "\tPosition: "
            << camera->GetPosition()[0] << ", "
            << camera->GetPosition()[1] << ", "
            << camera->GetPosition()[2];
  qDebug() << "\tFocal point: "
            << camera->GetFocalPoint()[0] << ", "
            << camera->GetFocalPoint()[1] << ", "
            << camera->GetFocalPoint()[2];
  qDebug() << "Viewup: " << camera->GetViewUp()[0] << ", "
           << camera->GetViewUp()[1] << ", "
           << camera->GetViewUp()[2];
}

CameraWindow::CameraWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::CameraWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Four cameras"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_topLeftRenderer = vtkRenderer::New();
    m_topLeftRenderer->SetBackground(1, 0, 0);
    m_topRightRenderer = vtkRenderer::New();
    m_topRightRenderer->SetBackground(0, 1, 0);
    m_bottomLeftRenderer = vtkRenderer::New();
    m_bottomLeftRenderer->SetBackground(0, 1, 1);
    m_bottomRightRenderer = vtkRenderer::New();
    m_bottomRightRenderer->SetBackground(1, 0, 1);
    m_vtkWidget->GetRenderWindow()->AddRenderer(m_topLeftRenderer);
    m_vtkWidget->GetRenderWindow()->AddRenderer(m_topRightRenderer);
    m_vtkWidget->GetRenderWindow()->AddRenderer(m_bottomLeftRenderer);
    m_vtkWidget->GetRenderWindow()->AddRenderer(m_bottomRightRenderer);

    m_actor = vtkActor::New();

    VTK_CREATE(vtkPlatonicSolidSource, source);
    source->SetSolidTypeToOctahedron();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(source->GetOutputPort());

    m_actor->SetMapper(mapper);
    m_topLeftRenderer->AddActor(m_actor);
    m_topRightRenderer->AddActor(m_actor);
    m_bottomLeftRenderer->AddActor(m_actor);
    m_bottomRightRenderer->AddActor(m_actor);

    vtkCamera* camera = m_topLeftRenderer->GetActiveCamera();
    m_topRightRenderer->SetActiveCamera(camera);
    m_bottomLeftRenderer->SetActiveCamera(camera);
    m_bottomRightRenderer->SetActiveCamera(camera);

    m_topLeftRenderer->SetViewport(0.0, 0.0, 0.5, 0.5);
    m_topRightRenderer->SetViewport(0.5, 0.0, 1.0, 0.5);
    m_bottomLeftRenderer->SetViewport(0.0, 0.5, 0.5, 1.0);
    m_bottomRightRenderer->SetViewport(0.5, 0.5, 1.0, 1.0);
    m_topLeftRenderer->ResetCamera();
    m_vtkWidget->showOrientationMarker();

    VTK_CREATE(vtkCallbackCommand, modifiedCallback);
    modifiedCallback->SetCallback(CameraModifiedCallback);
    m_topLeftRenderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, modifiedCallback);
}

CameraWindow::~CameraWindow()
{
    delete m_ui;
}
