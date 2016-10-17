#include "distancewidgetwindow.h"
#include "ui_distancewidgetconfig.h"
#include "ui_basewidgetwindow.h"

#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/distancewidgetobserver.h>

#include <vtkDistanceWidget.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkDistanceRepresentation3D.h>
#include <vtkRenderer.h>

DistanceWidgetWindow::DistanceWidgetWindow(QWidget* parent) : BaseWidgetWindow(parent)
{
    createUi();
    createWidget();
}

void DistanceWidgetWindow::createWidget()
{
    VtkUtils::vtkInitOnce(m_2dRep);
    VtkUtils::vtkInitOnce(m_2dWidget);
    m_2dWidget->SetRepresentation(m_2dRep);
    m_2dWidget->SetInteractor(m_vtkWidget->GetInteractor());
    m_2dWidget->On();
    hookWidget(m_2dWidget);
    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void DistanceWidgetWindow::createUi()
{
    m_configUi = new Ui::DistanceWidgetConfig;
    setupConfigWidget(m_configUi);

    BaseWidgetWindow::createUi();

    setWindowTitle(tr("Distance Widget"));
}

void DistanceWidgetWindow::on_typeCombo_currentIndexChanged(int index)
{
    if (index == 0) {
        m_2dWidget->On();

        if (m_3dWidget)
            m_3dWidget->Off();
    } else {
        if (!m_3dWidget) {
            VtkUtils::vtkInitOnce(m_3dRep);
            VtkUtils::vtkInitOnce(m_3dWidget);
            m_3dWidget->SetRepresentation(m_3dRep);
            m_3dWidget->SetInteractor(m_vtkWidget->GetInteractor());
            hookWidget(m_3dWidget);
        }
        m_3dWidget->On();
        m_2dWidget->Off();
    }

    m_vtkWidget->update();
}

void DistanceWidgetWindow::on_point1XSpinBox_valueChanged(double arg1)
{
    double pos[3];
    double newPos[3];
    newPos[0] = arg1;
    newPos[1] = pos[1];
    newPos[2] = pos[2];

    if (m_2dWidget->GetEnabled()) {
        m_2dRep->GetPoint1DisplayPosition(pos);
        m_2dRep->SetPoint1DisplayPosition(newPos);
    } else {
        m_3dRep->GetPoint1DisplayPosition(pos);
        m_3dRep->SetPoint1DisplayPosition(newPos);
    }
    m_vtkWidget->update();
}

void DistanceWidgetWindow::on_point1YSpinBox_valueChanged(double arg1)
{
    double pos[3];
    double newPos[3];
    newPos[0] = pos[0];
    newPos[1] = arg1;
    newPos[2] = pos[2];

    if (m_2dWidget->GetEnabled()) {
        m_2dRep->GetPoint1DisplayPosition(pos);
        m_2dRep->SetPoint1DisplayPosition(newPos);
    } else {
        m_3dRep->GetPoint1DisplayPosition(pos);
        m_3dRep->SetPoint1DisplayPosition(newPos);
    }
    m_vtkWidget->update();
}

void DistanceWidgetWindow::on_point1ZSpinBox_valueChanged(double arg1)
{
    double pos[3];
    double newPos[3];
    newPos[0] = pos[0];
    newPos[1] = pos[1];
    newPos[2] = arg1;

    if (m_2dWidget->GetEnabled()) {
        m_2dRep->GetPoint1DisplayPosition(pos);
        m_2dRep->SetPoint1DisplayPosition(newPos);
    } else {
        m_3dRep->GetPoint1DisplayPosition(pos);
        m_3dRep->SetPoint1DisplayPosition(newPos);
    }
    m_vtkWidget->update();
}

void DistanceWidgetWindow::on_point2XSpinBox_valueChanged(double arg1)
{
    double pos[3];
    double newPos[3];
    newPos[0] = arg1;
    newPos[1] = pos[1];
    newPos[2] = pos[2];

    if (m_2dWidget->GetEnabled()) {
        m_2dRep->GetPoint2DisplayPosition(pos);
        m_2dRep->SetPoint2DisplayPosition(newPos);
    } else {
        m_3dRep->GetPoint2DisplayPosition(pos);
        m_3dRep->SetPoint2DisplayPosition(newPos);
    }
    m_vtkWidget->update();
}

void DistanceWidgetWindow::on_point2YSpinBox_valueChanged(double arg1)
{
    double pos[3];
    double newPos[3];
    newPos[0] = pos[0];
    newPos[1] = arg1;
    newPos[2] = pos[2];

    if (m_2dWidget->GetEnabled()) {
        m_2dRep->GetPoint2DisplayPosition(pos);
        m_2dRep->SetPoint2DisplayPosition(newPos);
    } else {
        m_3dRep->GetPoint2DisplayPosition(pos);
        m_3dRep->SetPoint2DisplayPosition(newPos);
    }
    m_vtkWidget->update();
}

void DistanceWidgetWindow::on_point2ZSpinBox_valueChanged(double arg1)
{
    double pos[3];
    double newPos[3];
    newPos[0] = pos[0];
    newPos[1] = pos[1];
    newPos[2] = arg1;

    if (m_2dWidget->GetEnabled()) {
        m_2dRep->GetPoint2DisplayPosition(pos);
        m_2dRep->SetPoint2DisplayPosition(newPos);
    } else {
        m_3dRep->GetPoint2DisplayPosition(pos);
        m_3dRep->SetPoint2DisplayPosition(newPos);
    }
    m_vtkWidget->update();
}

void DistanceWidgetWindow::onDistanceChanged(double dist)
{
    m_configUi->distanceSpinBox->setValue(dist);
}

void DistanceWidgetWindow::onWorldPoint1Changed(double* pos)
{
    Utils::SignalBlocker sb(m_configUi->point1XSpinBox);
    sb.addObject(m_configUi->point1YSpinBox);
    sb.addObject(m_configUi->point1ZSpinBox);

    m_configUi->point1XSpinBox->setValue(pos[0]);
    m_configUi->point1YSpinBox->setValue(pos[1]);
    m_configUi->point1ZSpinBox->setValue(pos[2]);
}

void DistanceWidgetWindow::onWorldPoint2Changed(double* pos)
{
    Utils::SignalBlocker sb(m_configUi->point2XSpinBox);
    sb.addObject(m_configUi->point2YSpinBox);
    sb.addObject(m_configUi->point2ZSpinBox);

    m_configUi->point2XSpinBox->setValue(pos[0]);
    m_configUi->point2YSpinBox->setValue(pos[1]);
    m_configUi->point2ZSpinBox->setValue(pos[2]);
}

void DistanceWidgetWindow::hookWidget(const vtkSmartPointer<vtkDistanceWidget>& widget)
{
    VtkUtils::DistanceWidgetObserver* distWidgetObserver = new VtkUtils::DistanceWidgetObserver(this);
    distWidgetObserver->attach(widget);
    connect(distWidgetObserver, SIGNAL(distanceChanged(double)), this, SLOT(onDistanceChanged(double)));
    connect(distWidgetObserver, SIGNAL(worldPoint1Changed(double*)), this, SLOT(onWorldPoint1Changed(double*)));
    connect(distWidgetObserver, SIGNAL(worldPoint2Changed(double*)), this, SLOT(onWorldPoint2Changed(double*)));
}



