#include "camerawidgetwindow.h"
#include "ui_basewidgetwindow.h"
#include "ui_camerawidgetconfig.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkCameraWidget.h>
#include <vtkCameraRepresentation.h>
#include <vtkRenderer.h>

CameraWidgetWindow::CameraWidgetWindow(QWidget* parent) : BaseWidgetWindow(parent)
{
    createUi();
    createWidget();
}

void CameraWidgetWindow::createWidget()
{
    VtkUtils::vtkInitOnce(m_rep);
    VtkUtils::vtkInitOnce(m_widget);
    m_widget->SetRepresentation(m_rep);
    m_widget->SetInteractor(m_vtkWidget->GetInteractor());
    m_rep->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());
    m_widget->On();

    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void CameraWidgetWindow::createUi()
{
    m_configUi = new Ui::CameraWidgetConfig;
    setupConfigWidget(m_configUi);

    BaseWidgetWindow::createUi();

    setWindowTitle(tr("Camera Widget"));
}

