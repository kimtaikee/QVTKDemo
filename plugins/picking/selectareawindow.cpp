#include "selectareawindow.h"
#include "ui_selectareawindow.h"
#include "selectareainteractorstyle.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPlanes.h>
#include <vtkCylinderSource.h>
#include <vtkExtractGeometry.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkAreaPicker.h>
#include <vtkSphereSource.h>

#include <QDebug>

SelectAreaWindow::SelectAreaWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SelectAreaWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Select Area"));
    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_interactorStyle = SelectAreaInteractorStyle::New();
    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_interactorStyle);
    connect(m_interactorStyle, SIGNAL(areaSelected(vtkPlanes*)), this, SLOT(onAreaSelected(vtkPlanes*)));

    m_interactorStyle->GetInteractor()->SetPicker(vtkAreaPicker::New());

    m_selectionActor = vtkActor::New();
    m_modelActor = vtkActor::New();

    VTK_CREATE(vtkSphereSource, sphereSrc);
    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(sphereSrc->GetOutputPort());

    m_modelActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_modelActor);
}

SelectAreaWindow::~SelectAreaWindow()
{
    delete m_ui;
}

void SelectAreaWindow::onAreaSelected(vtkPlanes* plane)
{
    VTK_CREATE(vtkExtractGeometry, extractGeometry);
    extractGeometry->SetImplicitFunction(plane);
    extractGeometry->SetInputData(m_modelActor->GetMapper()->GetInput());
    extractGeometry->Update();

    VTK_CREATE(vtkVertexGlyphFilter, glyphFilter);
    glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
    glyphFilter->Update();

    vtkPolyData* selected = glyphFilter->GetOutput();
    qDebug() << "Selected " << selected->GetNumberOfPoints() << " points." ;
    qDebug() << "Selected " << selected->GetNumberOfCells() << " cells." ;

    VTK_CREATE(vtkDataSetMapper, mapper);
    mapper->SetInputData(selected);
    mapper->ScalarVisibilityOff();

    vtkIdTypeArray* ids = vtkIdTypeArray::SafeDownCast(selected->GetPointData()->GetArray("OriginalIds"));
    if (ids) {
        for(vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
        {
            qDebug() << "Id " << i << " : " << ids->GetValue(i) ;
        }
    }

    m_selectionActor->SetMapper(mapper);
    m_selectionActor->GetProperty()->SetColor(1.0, 0.0, 0.0); //(R,G,B)
    m_selectionActor->GetProperty()->SetPointSize(3);
    m_vtkWidget->update();
}
