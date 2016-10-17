#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MainWidget)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Messing with camera"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_actor = vtkActor::New();



}

MainWidget::~MainWidget()
{
    delete  m_ui;
}
