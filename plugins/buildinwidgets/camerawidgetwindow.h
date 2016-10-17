#ifndef CAMERAWIDGETWINDOW_H
#define CAMERAWIDGETWINDOW_H

#include "basewidgetwindow.h"

namespace Ui
{
    class CameraWidgetConfig;
}

class vtkCameraWidget;
class vtkCameraRepresentation;
class CameraWidgetWindow : public BaseWidgetWindow
{
    Q_OBJECT
public:
    explicit CameraWidgetWindow(QWidget* parent = nullptr);

protected:
    void createWidget();
    void createUi();

private:
    Ui::CameraWidgetConfig* m_configUi = nullptr;

    vtkSmartPointer<vtkCameraWidget> m_widget;
    vtkSmartPointer<vtkCameraRepresentation> m_rep;
};

#endif // CAMERAWIDGETWINDOW_H
