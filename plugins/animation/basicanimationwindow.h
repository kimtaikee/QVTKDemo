#ifndef BASICANIMATIONWINDOW_H
#define BASICANIMATIONWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class BasicAnimationWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class AnimationCallback;
class vtkActor;
class BasicAnimationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BasicAnimationWindow(QWidget *parent = 0);
    ~BasicAnimationWindow();

private slots:
    void on_intervalSpinBox_valueChanged(int arg1);
    void on_startAnimationButton_toggled(bool checked);

    void on_orbitAnimCheckBox_toggled(bool checked);

private:
    void init();
    void startTimer();
    void stopTimer();
    void addTextureToSphere(const vtkSmartPointer<vtkActor>& actor, const QString& textureFile);

private:
    Ui::BasicAnimationWindow *m_ui;
    int m_interval = 10; // ms
    int m_timerId = 0;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_animationActor;
    vtkSmartPointer<AnimationCallback> m_animCallback;
};

#endif // BASICANIMATIONWINDOW_H
