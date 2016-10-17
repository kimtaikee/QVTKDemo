#include "drawspherewindow.h"
#include "ui_drawspherewindow.h"
#include "mouseeventinteractorstyle.h"

#include <utils/utils.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkProperty.h>

#include <QKeyEvent>
#include <QDebug>

DrawSphereWindow::DrawSphereWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::DrawSphereWindow)
{
    m_ui->setupUi(this);
    m_colorSeries.setScheme(Utils::ColorSeries::Cool);

    m_interactorStyle = MouseEventInteractorStyle::New();
    m_interactorStyle->setMoveActor(true);
    connect(m_interactorStyle, SIGNAL(mouseMoved(double*)), this, SLOT(onMouseMoved(double*)));
    connect(m_interactorStyle, SIGNAL(mousePressed(double*)), this, SLOT(onMousePressed(double*)));
    connect(m_interactorStyle, SIGNAL(mouseReleased(double*)), this, SLOT(onMouseReleased(double*)));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_interactorStyle->SetDefaultRenderer(m_vtkWidget->defaultRenderer());
    m_vtkWidget->GetInteractor()->SetInteractorStyle(m_interactorStyle);
    m_vtkWidget->showOrientationMarker();

    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_promptActor = vtkTextActor::New();
    m_promptActor->SetInput("Press left mouse button click & drag to draw sphere, press right mouse button to rotate!");
    m_vtkWidget->defaultRenderer()->AddActor2D(m_promptActor);

    setWindowTitle(tr("Draw Sphere"));
}

DrawSphereWindow::~DrawSphereWindow()
{
    m_interactorStyle->Delete();
    delete m_ui;
}

void DrawSphereWindow::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);

    m_mousePressed = event->modifiers() & Qt::ControlModifier;
}

void DrawSphereWindow::keyReleaseEvent(QKeyEvent* event)
{
    QWidget::keyReleaseEvent(event);

    if (event->modifiers() & Qt::ControlModifier)
        m_mousePressed = false;
}

void DrawSphereWindow::onMousePressed(double* pos)
{
    Utils::ArrayAssigner<double>()(m_pressedPos, pos);

    m_currentSphereColor = m_colorSeries.nextColor();
    m_mousePressed = true;
    m_sphereSource = vtkSphereSource::New();
    m_sphereSource->SetThetaResolution(30);
    m_sphereSource->SetPhiResolution(30);
    m_sphereSource->SetCenter(m_pressedPos);
    m_currentSphereColor = m_colorSeries.nextColor();
}

void DrawSphereWindow::onMouseMoved(double* pos)
{
    Utils::ArrayAssigner<double>()(m_movedPos, pos);

    if (m_mousePressed) {

        double radius = Utils::distance<double>(m_pressedPos, pos);
        m_sphereSource->SetRadius(radius);

        VTK_CREATE(vtkPolyDataMapper, mapper);
        mapper->SetInputConnection(m_sphereSource->GetOutputPort());

        VtkUtils::vtkInitOnce(m_currentActor);
        m_currentActor->SetMapper(mapper);

        double clrArr[3];
        Utils::vtkColor(m_currentSphereColor, clrArr);
        m_currentActor->GetProperty()->SetColor(clrArr);

//        m_currentActor->SetPosition(m_pressedPos);
        m_vtkWidget->defaultRenderer()->AddActor(m_currentActor);
        m_vtkWidget->update();
    }
}

void DrawSphereWindow::onMouseReleased(double* pos)
{
    Q_UNUSED(pos)
    m_mousePressed = false;

    if (m_currentActor) {
        m_sphereSource->Delete();
        m_actorList.append(m_currentActor);
        m_currentActor = nullptr;
    }
}


