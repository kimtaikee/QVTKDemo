#include "galaxywindow.h"
#include "ui_galaxywindow.h"

#include <utils/utils.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/moveactorinteractorstyle.h>

#include <vtkLight.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkLightActor.h>
#include <vtkTextureMapToSphere.h>

GalaxyWindow::GalaxyWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::GalaxyWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Galaxy"));

    init();
}

GalaxyWindow::~GalaxyWindow()
{
    delete m_ui;
}

void GalaxyWindow::onColorChanged(const QColor &clr)
{
    Widgets::ColorPushButton* button = qobject_cast<Widgets::ColorPushButton*>(sender());

    double clrArr[3];
    Utils::vtkColor(clr, clrArr);
    if (button == m_ui->ambientColorButton)
        m_light->SetAmbientColor(clrArr);
    else if (button == m_ui->diffuseColorButton)
        m_light->SetDiffuseColor(clrArr);
    else if (button == m_ui->specularColorButton)
        m_light->SetSpecularColor(clrArr);

    m_vtkWidget->update();
}

void GalaxyWindow::onActorMoved(vtkActor* actor)
{
    if (actor == m_sunActor) {
//        m_light->SetPosition(m_sunActor->GetPosition());
//        m_light->SetFocalPoint(m_earthActor->GetPosition());
        m_vtkWidget->update();
    }
}

static vtkActor* createSphereActor(double radius)
{
    vtkActor* actor = vtkActor::New();

    VTK_CREATE(vtkSphereSource, sphere);
    sphere->SetRadius(radius);

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(sphere->GetOutputPort());
    actor->SetMapper(mapper);

    return actor;
}

void GalaxyWindow::init()
{
    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_interactorStyle = VtkUtils::MoveActorInteractorStyle::New();
    m_interactorStyle->SetDefaultRenderer(m_vtkWidget->defaultRenderer());
    m_interactorStyle->setHighlightActor(true);
    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_interactorStyle);
    connect(m_interactorStyle, SIGNAL(actorMoved(vtkActor*)), this, SLOT(onActorMoved(vtkActor*)));

    m_light = vtkLight::New();
    m_light->SetAmbientColor(0.3, 0.5, 0.7);
    m_light->SetPosition(10, 10, 4);
    m_vtkWidget->defaultRenderer()->AddLight(m_light);
    m_light->SetFocalPoint(0, 0, 0);

    m_lightActor = vtkLightActor::New();
    m_lightActor->SetLight(m_light);
    m_vtkWidget->defaultRenderer()->AddViewProp(m_lightActor);

    m_earthActor = createSphereActor(0.2);
    m_sunActor = createSphereActor(11);
    m_vtkWidget->defaultRenderer()->AddActor(m_sunActor);
    m_sunActor->SetPosition(.0, .0, .0);
    m_vtkWidget->defaultRenderer()->AddActor(m_earthActor);
    m_earthActor->SetPosition(3.2, 1.5, 5.4);

    setActorTexture(m_earthActor, 2, "texture_earth");
    setActorTexture(m_sunActor, 10, "texture_sun");

    connect(m_ui->ambientColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->diffuseColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->specularColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));

    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void GalaxyWindow::setActorTexture(vtkActor* actor, double radius, const QString& textureFile)
{
    QImage image;
    image.load(QString(":%1").arg(textureFile));

    VTK_CREATE(vtkImageData, vtkImgData);
    VtkUtils::qImageToVtkImage(image, vtkImgData);

    VTK_CREATE(vtkTexture, texture);
    texture->SetInputData(vtkImgData);
    texture->Update();

    VTK_CREATE(vtkSphereSource, sphereSource);
    sphereSource->SetThetaResolution(30);
    sphereSource->SetPhiResolution(30);
    sphereSource->SetRadius(radius);
    sphereSource->Update();

    VTK_CREATE(vtkTextureMapToSphere, mapToSphere);
    mapToSphere->SetInputData(sphereSource->GetOutput());
    mapToSphere->PreventSeamOn();
    mapToSphere->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(mapToSphere->GetOutputPort());

    actor->SetMapper(mapper);
    actor->SetTexture(texture);
}

void GalaxyWindow::on_typeCombo_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        m_light->SetLightTypeToHeadlight();
        break;

    case 1:
        m_light->SetLightTypeToCameraLight();
        break;

    case 2:
        m_light->SetLightTypeToSceneLight();
        break;
    }

    m_vtkWidget->update();
}

void GalaxyWindow::on_groupBox_3_toggled(bool arg1)
{
    m_light->SetSwitch(arg1);
    m_vtkWidget->update();
}

void GalaxyWindow::on_coneAngleSpinBox_valueChanged(double arg1)
{
    m_light->SetConeAngle(arg1);
}
