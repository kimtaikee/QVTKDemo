#include "pipelinewindow.h"
#include "ui_pipelinewindow.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include <vtkCylinder.h>

#include <QMdiArea>
#include <QMdiSubWindow>

PipelineWindow::PipelineWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PipelineWindow)
{
    m_mdiArea = new QMdiArea(this);
    m_ui->setupUi(this);
    setWindowTitle(tr("Pipeline"));
    m_ui->previewLayout->addWidget(m_mdiArea);

    for (int i = 0; i < 10; i++) {
        QMdiSubWindow* subWindow = new QMdiSubWindow(this);
        VtkUtils::VtkWidget* vtkWidget = new VtkUtils::VtkWidget(this);
        vtkWidget->defaultRenderer()->AddActor(VtkUtils::createSourceActor<vtkCylinderSource>());
        subWindow->setWidget(vtkWidget);
        m_mdiArea->addSubWindow(subWindow);
    }
}

PipelineWindow::~PipelineWindow()
{
    delete m_ui;
}
