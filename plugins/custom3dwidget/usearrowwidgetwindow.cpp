#include "usearrowwidgetwindow.h"
#include "ui_usearrowwidgetwindow.h"
#include "vtkarrowrepresentation.h"
#include "vtkarrowwidget.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkRenderer.h>
#include <vtkConeSource.h>

UseArrowWidgetWindow::UseArrowWidgetWindow(QWidget *parent) : QWidget(parent)
{
    m_ui = new Ui::UseArrowWidgetWindow;
    m_ui->setupUi(this);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_vtkWidget->defaultRenderer()->AddActor(VtkUtils::createSourceActor<vtkConeSource>());
    m_vtkWidget->defaultRenderer()->SetBackground(0.3, 0.5, .0);

    m_widget = vtkSmartPointer<vtkArrowWidget>::New();
    m_widget->CreateDefaultRepresentation();
    m_widget->GetRepresentation()->SetRenderer(m_vtkWidget->defaultRenderer());
    m_widget->SetInteractor(m_vtkWidget->GetInteractor());
    m_widget->On();

    setWindowTitle(tr("Use Arrow Widget"));
}

void UseArrowWidgetWindow::on_pot1xSpinBox_valueChanged(double arg1)
{
    double pos[3];
    pos[0] = arg1;
    pos[1] = m_ui->pot1ySpinBox->value();
    pos[2] = m_ui->pot1zSpinBox->value();

    vtkArrowRepresentation* rep = vtkArrowRepresentation::SafeDownCast(m_widget->GetRepresentation());
    rep->SetPoint1WorldPosition(pos);
    m_vtkWidget->update();
}

void UseArrowWidgetWindow::on_pot1ySpinBox_valueChanged(double arg1)
{
    double pos[3];
    pos[0] = m_ui->pot1xSpinBox->value();
    pos[1] = arg1;
    pos[2] = m_ui->pot1zSpinBox->value();

    vtkArrowRepresentation* rep = vtkArrowRepresentation::SafeDownCast(m_widget->GetRepresentation());
    rep->SetPoint1WorldPosition(pos);
    m_vtkWidget->update();
}

void UseArrowWidgetWindow::on_pot1zSpinBox_valueChanged(double arg1)
{
    double pos[3];
    pos[0] = m_ui->pot1xSpinBox->value();
    pos[1] = m_ui->pot1ySpinBox->value();
    pos[2] = arg1;

    vtkArrowRepresentation* rep = vtkArrowRepresentation::SafeDownCast(m_widget->GetRepresentation());
    rep->SetPoint1WorldPosition(pos);
    m_vtkWidget->update();
}

void UseArrowWidgetWindow::on_pot2xSpinBox_valueChanged(double arg1)
{
    double pos[3];
    pos[0] = arg1;
    pos[1] = m_ui->pot2ySpinBox->value();
    pos[2] = m_ui->pot2zSpinBox->value();

    vtkArrowRepresentation* rep = vtkArrowRepresentation::SafeDownCast(m_widget->GetRepresentation());
    rep->SetPoint2WorldPosition(pos);
    m_vtkWidget->update();
}

void UseArrowWidgetWindow::on_pot2ySpinBox_valueChanged(double arg1)
{
    double pos[3];
    pos[0] = m_ui->pot2xSpinBox->value();
    pos[1] = arg1;
    pos[2] = m_ui->pot2zSpinBox->value();

    vtkArrowRepresentation* rep = vtkArrowRepresentation::SafeDownCast(m_widget->GetRepresentation());
    rep->SetPoint2WorldPosition(pos);
    m_vtkWidget->update();
}

void UseArrowWidgetWindow::on_pot2zSpinBox_valueChanged(double arg1)
{
    double pos[3];
    pos[0] = m_ui->pot2xSpinBox->value();
    pos[1] = m_ui->pot2ySpinBox->value();
    pos[2] = arg1;

    vtkArrowRepresentation* rep = vtkArrowRepresentation::SafeDownCast(m_widget->GetRepresentation());
    rep->SetPoint2WorldPosition(pos);
    m_vtkWidget->update();
}
