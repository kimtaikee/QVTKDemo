#include "animationscenewindow.h"
#include "ui_animationscenewindow.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAnimationScene.h>
#include <vtkAnimationCue.h>
#include <vtkCamera.h>

class ActorAnimator
{
public:
    ActorAnimator()
    {
        this->Actor=0;
        this->Observer = AnimationCueObserver::New();
        this->Observer->Animator = this;
        this->StartPosition.resize(3);
        this->StartPosition.insert(this->StartPosition.begin(), 3, 0.0);
        this->EndPosition.resize(3);
        this->EndPosition.insert(this->EndPosition.begin(), 3, .5);
    }

    ~ActorAnimator()
    {
        if(this->Actor)
        {
            this->Actor->UnRegister(0);
            this->Actor=0;
        }
        this->Observer->UnRegister(0);
    }
    void SetActor(vtkActor *actor)
    {
        if (this->Actor)
        {
            this->Actor->UnRegister(0);
        }
        this->Actor = actor;
        this->Actor->Register(0);
    }
    void SetStartPosition(std::vector<double> position)
    {
        this->StartPosition = position;
    }
    void SetEndPosition(std::vector<double> position)
    {
        this->EndPosition = position;
    }
    void AddObserversToCue(vtkAnimationCue *cue)
    {
        cue->AddObserver(vtkCommand::StartAnimationCueEvent,this->Observer);
        cue->AddObserver(vtkCommand::EndAnimationCueEvent,this->Observer);
        cue->AddObserver(vtkCommand::AnimationCueTickEvent,this->Observer);
    }

    void Start(vtkAnimationCue::AnimationCueInfo *vtkNotUsed(info))
    {
        this->Actor->SetPosition(this->StartPosition[0],
                this->StartPosition[1],
                this->StartPosition[2]);
    }

    void Tick(vtkAnimationCue::AnimationCueInfo *info)
    {
        double t = (info->AnimationTime - info->StartTime) / (info->EndTime - info->StartTime);
        double position[3];
        for (int i = 0; i < 3; i++)
        {
            position[i] = this->StartPosition[i] + (this->EndPosition[i] - this->StartPosition[i]) * t;
        }
        this->Actor->SetPosition(position);

    }

    void End(vtkAnimationCue::AnimationCueInfo *vtkNotUsed(info))
    {
        this->Actor->SetPosition(this->EndPosition[0],
                this->EndPosition[1],
                this->EndPosition[2]);
    }

protected:
    class AnimationCueObserver : public vtkCommand
    {
    public:
        static AnimationCueObserver *New()
        {
            return new AnimationCueObserver;
        }

        virtual void Execute(vtkObject *vtkNotUsed(caller),
                             unsigned long event,
                             void *calldata)
        {
            if(this->Animator != 0)
            {
                vtkAnimationCue::AnimationCueInfo *info=
                        static_cast<vtkAnimationCue::AnimationCueInfo *>(calldata);
                switch(event)
                {
                case vtkCommand::StartAnimationCueEvent:
                    this->Animator->Start(info);
                    break;
                case vtkCommand::EndAnimationCueEvent:
                    this->Animator->End(info);
                    break;
                case vtkCommand::AnimationCueTickEvent:
                    this->Animator->Tick(info);
                    break;
                }
            }
        }

        AnimationCueObserver()
        {
            this->Animator = 0;
        }
        ActorAnimator *Animator;
    };

    vtkActor *             Actor;
    AnimationCueObserver * Observer;
    std::vector<double>    StartPosition;
    std::vector<double>    EndPosition;
};

class AnimationSceneObserver : public vtkCommand
{
public:
    static AnimationSceneObserver *New()
    {
        return new AnimationSceneObserver;
    }

    void SetRenderWindow( vtkRenderWindow *renWin)
    {
        if (this->RenderWindow)
        {
            this->RenderWindow->UnRegister(this);
        }
        this->RenderWindow = renWin;
        this->RenderWindow->Register(this);

    }
    virtual void Execute(vtkObject *vtkNotUsed(caller),
                         unsigned long event,
                         void *vtkNotUsed(calldata))
    {
        if(this->RenderWindow != 0)
        {
            switch(event)
            {
            case vtkCommand::AnimationCueTickEvent:
                this->RenderWindow->Render();
                break;
            }
        }
    }

protected:
    AnimationSceneObserver()
    {
        this->RenderWindow = 0;
    }
    ~AnimationSceneObserver()
    {
        if(this->RenderWindow)
        {
            this->RenderWindow->UnRegister(this);
            this->RenderWindow = 0;
        }
    }
    vtkRenderWindow *RenderWindow;
};
AnimationSceneWindow::AnimationSceneWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AnimationSceneWindow)
{
    m_ui->setupUi(this);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_sphereActor = VtkUtils::createSourceActor<vtkSphereSource>();
    m_coneActor = VtkUtils::createSourceActor<vtkConeSource>();
    m_vtkWidget->defaultRenderer()->AddActor(m_sphereActor);
    m_vtkWidget->defaultRenderer()->AddActor(m_coneActor);

    // Create an Animation Scene
    VtkUtils::vtkInitOnce(m_scene);
    m_scene->SetModeToRealTime();
    //    m_scene->SetModeToSequence();
    m_scene->SetLoop(0);
    m_scene->SetFrameRate(5);
    m_scene->SetStartTime(0);
    m_scene->SetEndTime(20);

    VTK_CREATE(AnimationSceneObserver, sceneObserver);
    sceneObserver->SetRenderWindow(m_vtkWidget->GetRenderWindow());
    m_scene->AddObserver(vtkCommand::AnimationCueTickEvent,sceneObserver);

    // Create an Animation Cue for each actor
    VTK_CREATE(vtkAnimationCue, cue1);
    cue1->SetStartTime(5);
    cue1->SetEndTime(23);
    m_scene->AddCue(cue1);

    VTK_CREATE(vtkAnimationCue, cue2);
    cue2->SetStartTime(1);
    cue2->SetEndTime(10);
    m_scene->AddCue(cue2);

    // Create an ActorAnimator for each actor;
    ActorAnimator* animateSphere = new ActorAnimator;
    animateSphere->SetActor(m_sphereActor);
    animateSphere->AddObserversToCue(cue1);

    ActorAnimator* animateCone = new ActorAnimator;
    std::vector<double> endCone(3);
    endCone[0] = -1;
    endCone[1] = -1;
    endCone[2] = -1;
    animateCone->SetEndPosition(endCone);
    animateCone->SetActor(m_coneActor);
    animateCone->AddObserversToCue(cue2);

    m_vtkWidget->defaultRenderer()->GetActiveCamera()->Dolly(.5);
    m_vtkWidget->defaultRenderer()->ResetCameraClippingRange();
}

AnimationSceneWindow::~AnimationSceneWindow()
{
    delete m_ui;
}

void AnimationSceneWindow::on_startButton_toggled(bool checked)
{
    m_ui->startButton->setText(checked ? tr("Stop") : tr("Start"));
    if (checked)
        m_scene->Play();
    else
        m_scene->Stop();
}
