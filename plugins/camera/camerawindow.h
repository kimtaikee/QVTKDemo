#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui {
class CameraWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkRenderer;
class vtkActor;
class CameraWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWindow(QWidget *parent = 0);
    ~CameraWindow();

private:
    Ui::CameraWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_actor;
    vtkSmartPointer<vtkRenderer> m_topLeftRenderer;
    vtkSmartPointer<vtkRenderer> m_topRightRenderer;
    vtkSmartPointer<vtkRenderer> m_bottomLeftRenderer;
    vtkSmartPointer<vtkRenderer> m_bottomRightRenderer;
};

#endif // CAMERAWINDOW_H
