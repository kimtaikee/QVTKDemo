#include "custominteractorstyle.h"
#include "../graph3d.h"
#include "util.h"

#include <vtkSmartPointer.h>
#include <vtkCellPicker.h>
#include <vtkActor.h>
#include <vtkProperty.h>

static const QColor HighlightColor = Qt::red;
static const QColor NormalColor = Qt::white;

CustomInteractorStyle::CustomInteractorStyle()
{

}

CustomInteractorStyle::~CustomInteractorStyle()
{

}

void CustomInteractorStyle::OnLeftButtonDown()
{
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    vtkActor* pickActor = this->pickedActor();
    if (pickActor)
        m_pickedActor = pickActor;
    else
        m_pickedActor = nullptr;
}

void CustomInteractorStyle::OnLeftButtonUp()
{
    if (m_pickedActor)
        emit mouseReleased();

    m_pickedActor = nullptr;
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void CustomInteractorStyle::Rotate()
{
    if (m_pickedActor)
        return;

    vtkInteractorStyleTrackballCamera::Rotate();
}

void CustomInteractorStyle::Spin()
{
    if (m_pickedActor)
        return;

    vtkInteractorStyleTrackballCamera::Spin();
}

void CustomInteractorStyle::OnMouseMove()
{
    vtkInteractorStyleTrackballCamera::OnMouseMove();
    notifyHoverActor();
}

void CustomInteractorStyle::OnChar()
{
    vtkInteractorStyleTrackballCamera::OnChar();
    char ch = this->GetInteractor()->GetKeyCode();
}

void CustomInteractorStyle::setGraph(ComDataGui3D::Graph3D *graph)
{
    m_graph = graph;
}

void CustomInteractorStyle::setClickPosition(const QPoint& pos)
{
    m_clickPos = pos;
}

void CustomInteractorStyle::setMovePosition(const QPoint& pos)
{
    m_movePos = pos;
}

void CustomInteractorStyle::setBounds(double bounds[])
{
    ComDataGui3D::detail::ArrayAssigner<double, 6>()(m_bounds, bounds);
}

vtkActor* CustomInteractorStyle::pickedActor()
{
    int x = m_clickPos.x();
    int y = m_graph->height() - m_clickPos.y();

    CREATE_VTKOBJ(vtkCellPicker, picker);
    picker->SetTolerance(0.001);
    picker->Pick(x, y, 0, GetDefaultRenderer());
    return picker->GetActor();
}

vtkActor* CustomInteractorStyle::hoverdActor()
{
    int x = m_movePos.x();
    int y = m_graph->height() - m_movePos.y();

    CREATE_VTKOBJ(vtkCellPicker, picker);
    picker->SetTolerance(0.001);
    picker->Pick(x, y, 0, GetDefaultRenderer());
    return picker->GetActor();
}

void CustomInteractorStyle::setPickingCursor()
{
    m_graph->setCursor(QCursor(Qt::PointingHandCursor));
}

void CustomInteractorStyle::resetPickingCursor()
{
    m_graph->setCursor(QCursor(Qt::ArrowCursor));
}

double* CustomInteractorStyle::computeBoundedPos(double *pos)
{
    double x = pos[0];
    double y = pos[1];
    double z = pos[2];

    m_boundedPos[0] = ComDataGui3D::detail::boundedValue(x, m_bounds[0], m_bounds[1]);
    m_boundedPos[1] = ComDataGui3D::detail::boundedValue(y, m_bounds[2], m_bounds[3]);
    m_boundedPos[2] = ComDataGui3D::detail::boundedValue(z, m_bounds[4], m_bounds[5]);

    return m_boundedPos;
}

void CustomInteractorStyle::addWatchActor(vtkActor *actor)
{
    m_watchedActorList.append(actor);
}

void CustomInteractorStyle::notifyHoverActor()
{
    vtkActor* hoverActor = this->hoverdActor();
    bool find = false;
    if (hoverActor) {
        foreach (vtkActor* actor, m_watchedActorList) {
            if (actor == hoverActor) {
                find = true;
                setPickingCursor();
                highlightActor(actor, true);
            }
        }

    } else {
        foreach (vtkActor* actor, m_watchedActorList)
            highlightActor(actor, false);
    }

    if (!find)
        resetPickingCursor();
}

void CustomInteractorStyle::highlightActor(vtkActor *actor, bool highlight)
{
    if (highlight) {
        actor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(HighlightColor));
    } else {
        QColor oClr = ComDataGui3D::detail::Util::oppositeColor(this->m_graph->backgroundColor());
        actor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(oClr));
    }
    m_graph->update();
}

