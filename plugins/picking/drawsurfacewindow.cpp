#include "drawsurfacewindow.h"
#include "ui_drawsurfacewindow.h"
#include "mouseeventinteractorstyle.h"

#include <utils/tablemodel.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkRenderWindow.h>
#include <vtkDelaunay2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkPointSource.h>
#include <vtkTextActor.h>
#include <vtkReverseSense.h>
#include <vtkContourFilter.h>
#include <vtkSurfaceReconstructionFilter.h>

#include <QDebug>
#include <QFileDialog>

DrawSurfaceWindow::DrawSurfaceWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::DrawSurfaceWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Draw Surface"));

    m_colorSeries.setScheme(Utils::ColorSeries::Spectrum);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_interactorStyle = MouseEventInteractorStyle::New();
    m_interactorStyle->SetDefaultRenderer(m_vtkWidget->defaultRenderer());
    connect(m_interactorStyle, SIGNAL(ctrlPressed(bool)), this, SLOT(onCtrlPressed(bool)));
    connect(m_interactorStyle, SIGNAL(mousePressed(double*)), this, SLOT(onMousePressed(double*)));
    m_vtkWidget->GetInteractor()->SetInteractorStyle(m_interactorStyle);
    m_vtkWidget->showOrientationMarker();

    m_promptActor = vtkTextActor::New();
    m_promptActor->SetInput("Press 'Ctrl' to start drawing, mouse click to select points, release 'ctrl' to draw surface");
    m_vtkWidget->defaultRenderer()->AddActor(m_promptActor);

    m_points = vtkPoints::New();
    m_surfaceActor = vtkActor::New();
    m_vtkWidget->defaultRenderer()->AddActor(m_surfaceActor);
}

DrawSurfaceWindow::~DrawSurfaceWindow()
{
    removePointActors();
    delete m_ui;
}

void DrawSurfaceWindow::on_resetButton_clicked()
{
//    m_tableModel->clear();
    m_points->Initialize();
    m_vtkWidget->update();
}

void DrawSurfaceWindow::addPoint(double* pos)
{

}

void DrawSurfaceWindow::onCtrlPressed(bool pressed)
{
    m_ctrlPressed = pressed;

    // construct surface when ctrl's released
    if (!pressed) {
        VTK_CREATE(vtkPolyData, polyData);
        polyData->SetPoints(m_points);

#if 1
        VTK_CREATE(vtkDelaunay2D, del);
        del->SetInputData(polyData);
#else
          // Construct the surface and create isosurface.
        VTK_CREATE(vtkSurfaceReconstructionFilter, surf);
        surf->SetInputData(polyData);

        VTK_CREATE(vtkContourFilter, cf);
        cf->SetInputConnection(surf->GetOutputPort());
        cf->SetValue(0, 0.0);

        // Sometimes the contouring algorithm can create a volume whose gradient
        // vector and ordering of polygon (using the right hand rule) are
        // inconsistent. vtkReverseSense cures this problem.
        VTK_CREATE(vtkReverseSense, reverse);
        reverse->SetInputConnection(cf->GetOutputPort());
        reverse->ReverseCellsOn();
        reverse->ReverseNormalsOn();

#endif
        VTK_CREATE(vtkPolyDataMapper, mapper);
        mapper->SetInputConnection(del->GetOutputPort());

        m_surfaceActor->SetMapper(mapper);
        m_vtkWidget->update();

    } else {
        // reset points
        m_points->Initialize();
        removePointActors();
    }
}

void DrawSurfaceWindow::onMousePressed(double* pos)
{
    if (m_ctrlPressed) {
//        m_tableModel->
        m_points->InsertNextPoint(pos[0], pos[1], pos[2]);

        VTK_CREATE(vtkSphereSource, ss);
        ss->SetThetaResolution(30);
        ss->SetPhiResolution(30);
        ss->SetRadius(0.005);
        ss->SetCenter(pos);

        VTK_CREATE(vtkPolyDataMapper, mapper);
        mapper->SetInputConnection(ss->GetOutputPort());

        double clrArr[3];
        Utils::vtkColor(m_colorSeries.nextColor(), clrArr);

        vtkActor* pointActor = vtkActor::New();
        pointActor->SetMapper(mapper);
        pointActor->GetProperty()->SetColor(clrArr);
        m_pointActors.append(pointActor);

        m_vtkWidget->defaultRenderer()->AddActor(pointActor);
        m_vtkWidget->update();
    }
}

void DrawSurfaceWindow::removePointActors()
{
    foreach (vtkActor* actor, m_pointActors) {
        m_vtkWidget->defaultRenderer()->RemoveActor(actor);
        actor->Delete();
    }
    m_pointActors.clear();
}

void DrawSurfaceWindow::on_exportButton_clicked()
{
    QString exportFile = QFileDialog::getSaveFileName(this, tr("Export Surface"), ".", "VRML Files(*.vrml)");
    if (exportFile.isEmpty())
        return;

    m_ui->exportFileEdit->setText(exportFile);
    VtkUtils::exportActorToFile(m_surfaceActor, exportFile);
}
