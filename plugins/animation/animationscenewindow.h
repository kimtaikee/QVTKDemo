#ifndef ANIMATIONSCENEWINDOW_H
#define ANIMATIONSCENEWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class AnimationSceneWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkAnimationScene;
class AnimationSceneWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AnimationSceneWindow(QWidget *parent = 0);
    ~AnimationSceneWindow();

private slots:
    void on_startButton_toggled(bool checked);

private:
    Ui::AnimationSceneWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkAnimationScene> m_scene;
    vtkSmartPointer<vtkActor> m_sphereActor;
    vtkSmartPointer<vtkActor> m_coneActor;
};

#endif // ANIMATIONSCENEWINDOW_H
