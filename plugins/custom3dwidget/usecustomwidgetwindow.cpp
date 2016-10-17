#include "usecustomwidgetwindow.h"
#include "ui_usecustomwidgetwindow.h"
#include "customxyplotwidget.h"
#include "customxyplotrepresentation.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkRenderer.h>
#include <vtkCylinderSource.h>

UseCustomWidgetWindow::UseCustomWidgetWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::UseCustomWidgetWindow)
{
    m_ui->setupUi(this);
    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_vtkWidget->defaultRenderer()->AddActor(VtkUtils::createSourceActor<vtkCylinderSource>());
    m_vtkWidget->defaultRenderer()->SetBackground(0.3, 0.5, .0);

    m_widget = vtkSmartPointer<CustomXYPlotWidget>::New();
    m_rep = vtkSmartPointer<CustomXYPlotRepresentation>::New();
    m_rep->SetRenderer(m_vtkWidget->defaultRenderer());
    m_widget->SetResizable(true);
    m_widget->SetSelectable(false);

    m_widget->SetRepresentation(m_rep);

    m_widget->CreateDefaultRepresentation();
    m_widget->SetInteractor(m_vtkWidget->GetInteractor());
    m_widget->On();
}

UseCustomWidgetWindow::~UseCustomWidgetWindow()
{
    delete m_ui;
}
