#include "pickcellwindow.h"
#include "ui_pickcellwindow.h"
#include "pickcellinteractorstyle.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkSphereSource.h>

#include <QDebug>

PickCellWindow::PickCellWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PickCellWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Pick Cell"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_interactorStyle = PickCellInteractorStyle::New();
    m_interactorStyle->SetDefaultRenderer(m_vtkWidget->defaultRenderer());
    connect(m_interactorStyle, SIGNAL(cellPicked(int)), this, SLOT(onCellPicked(int)));
    m_vtkWidget->GetInteractor()->SetInteractorStyle(m_interactorStyle);

    m_selectionActor = vtkActor::New();
    m_modelActor = vtkActor::New();

    VTK_CREATE(vtkSphereSource, sphereSrc);
    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(sphereSrc->GetOutputPort());

    m_modelActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_modelActor);
}

PickCellWindow::~PickCellWindow()
{
    delete m_ui;
}

void PickCellWindow::onCellPicked(int cellId)
{
    if (cellId < 0) {
        qDebug() << "invalid cell id.";
        return;
    }

    VTK_CREATE(vtkIdTypeArray, ids);
    ids->SetNumberOfComponents(1);
    ids->InsertNextValue(cellId);

    VTK_CREATE(vtkSelectionNode, selectionNode);
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids);

    VTK_CREATE(vtkSelection, selection);
    selection->AddNode(selectionNode);

    VTK_CREATE(vtkExtractSelection, extractSelection);
    extractSelection->SetInputData(0, m_modelActor->GetMapper()->GetInput());
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();

    // In selection
    VTK_CREATE(vtkUnstructuredGrid, selected);
    selected->ShallowCopy(extractSelection->GetOutput());

    VTK_CREATE(vtkDataSetMapper, selectedMapper);
    selectedMapper->SetInputData(selected);

    m_selectionActor->SetMapper(selectedMapper);
    m_selectionActor->GetProperty()->EdgeVisibilityOn();
    m_selectionActor->GetProperty()->SetEdgeColor(1,0,0);
    m_selectionActor->GetProperty()->SetLineWidth(2);
    m_vtkWidget->defaultRenderer()->AddActor(m_selectionActor);
    m_vtkWidget->update();
}
