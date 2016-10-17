#include "anglewidgetwindow.h"
#include "ui_anglewidgetconfig.h"
#include "ui_basewidgetwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkAngleWidget.h>
#include <vtkAngleRepresentation2D.h>
#include <vtkAngleRepresentation3D.h>
#include <vtkRenderer.h>

AngleWidgetWindow::AngleWidgetWindow(QWidget* parent) : BaseWidgetWindow(parent)
{
    createUi();
    createWidget();
}

void AngleWidgetWindow::createWidget()
{
    VtkUtils::vtkInitOnce(m_2dRep);
    VtkUtils::vtkInitOnce(m_2dWidget);
    m_2dWidget->SetRepresentation(m_2dRep);
    m_2dWidget->SetInteractor(m_vtkWidget->GetInteractor());
    m_2dWidget->On();

    m_vtkWidget->defaultRenderer()->ResetCamera();

}

void AngleWidgetWindow::createUi()
{
    m_configUi = new Ui::AngleWidgetConfig;
    setupConfigWidget(m_configUi);

    BaseWidgetWindow::createUi();

    setWindowTitle(tr("Angle Widget"));
}

void AngleWidgetWindow::on_typeCombo_currentIndexChanged(int index)
{
    if (index == 0) {
        m_2dWidget->On();

        if (m_3dWidget)
            m_3dWidget->Off();
    } else {
        VtkUtils::vtkInitOnce(m_3dRep);
        VtkUtils::vtkInitOnce(m_3dWidget);
        m_3dWidget->SetRepresentation(m_3dRep);
        m_3dWidget->SetInteractor(m_vtkWidget->GetInteractor());
        m_3dWidget->On();
        m_2dWidget->Off();
    }

    m_vtkWidget->update();
}
