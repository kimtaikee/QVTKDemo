#include "basicanimationwindow.h"
#include "ui_basicanimationwindow.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTexture.h>
#include <vtkImageData.h>

#include <QDebug>
#include <QFile>

const float PI = 3.14159;
const float DEG2RAD = PI/180;

class AnimationCallback : public vtkCommand
{
public:
    static AnimationCallback *New()
    {
        AnimationCallback *cb = new AnimationCallback;
        cb->TimerCount = 0;
        return cb;
    }

    virtual void Execute(vtkObject *caller, unsigned long eventId,
                         void * vtkNotUsed(callData))
    {

        if (vtkCommand::TimerEvent == eventId)
            ++this->TimerCount;


        if (UseOrbit) {
            double degreeInRad = (this->TimerCount % 360) * DEG2RAD;
            double x = cos(degreeInRad) * Radius;
            double y = sin(degreeInRad) * Radius;
            actor->SetPosition(this->TimerCount / 10.0, this->TimerCount / 10.0, 0);
        }

//        actor->SetPosition(x, y, 0);
        actor->RotateZ(0.1);
        vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
        iren->GetRenderWindow()->Render();
    }

private:
    int TimerCount;
    double Radius = 2.0;

public:
    bool UseOrbit = false;
    vtkActor* actor;
};

BasicAnimationWindow::BasicAnimationWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::BasicAnimationWindow)
{
    init();
}

BasicAnimationWindow::~BasicAnimationWindow()
{
    delete m_ui;
}

void BasicAnimationWindow::init()
{
    m_ui->setupUi(this);
    m_ui->intervalSpinBox->setValue(m_interval);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    VTK_CREATE(vtkSphereSource, sphere);
    sphere->SetThetaResolution(30);
    sphere->SetPhiResolution(30);

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(sphere->GetOutputPort());

    VtkUtils::vtkInitOnce(m_animationActor);
    m_animationActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_animationActor);
    m_animationActor->SetPosition(5.0, 0.0, 0.0);
    addTextureToSphere(m_animationActor, ":earth_surface");

    VtkUtils::vtkInitOnce(m_animCallback);
    m_animCallback->actor = m_animationActor;
    m_vtkWidget->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::TimerEvent, m_animCallback);
}

void BasicAnimationWindow::startTimer()
{
    m_timerId = m_vtkWidget->GetRenderWindow()->GetInteractor()->CreateRepeatingTimer(m_interval);
}

void BasicAnimationWindow::stopTimer()
{
    m_vtkWidget->GetRenderWindow()->GetInteractor()->DestroyTimer(m_timerId);
}

void BasicAnimationWindow::addTextureToSphere(const vtkSmartPointer<vtkActor>& actor, const QString& textureFile)
{
    QFile imgFile(textureFile);
    if (!imgFile.open(QIODevice::ReadOnly)) {
        qDebug() << "failed to read default texture file.";
        return;
    }

    QImage textureImg = QImage::fromData(imgFile.readAll());

    vtkImageData* imgData = vtkImageData::New();
    VtkUtils::qImageToVtkImage(textureImg, imgData);

    VTK_CREATE(vtkTexture, texture);
    texture->SetInputData(imgData);
    texture->Update();

    VTK_CREATE(vtkSphereSource, sphereSource);
    sphereSource->SetThetaResolution(30);
    sphereSource->SetPhiResolution(30);
    sphereSource->Update();

    VTK_CREATE(vtkTextureMapToSphere, mapToSphere);
    mapToSphere->SetInputData(sphereSource->GetOutput());
    mapToSphere->PreventSeamOn();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(mapToSphere->GetOutputPort());

    actor->SetMapper(mapper);
    actor->SetTexture(texture);
}

void BasicAnimationWindow::on_intervalSpinBox_valueChanged(int arg1)
{
    m_interval = arg1;
    if (m_ui->startAnimationButton->isChecked()) {
        stopTimer();
        startTimer();
    }
}

void BasicAnimationWindow::on_startAnimationButton_toggled(bool checked)
{
    m_ui->startAnimationButton->setText(checked ? tr("Stop") : tr("Start"));
    if (checked)
        startTimer();
    else
        stopTimer();
}

void BasicAnimationWindow::on_orbitAnimCheckBox_toggled(bool checked)
{
    stopTimer();
    m_animCallback->UseOrbit = checked;
    startTimer();
}
