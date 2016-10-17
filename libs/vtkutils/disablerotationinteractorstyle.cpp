#include "disablerotationinteractorstyle.h"
#include "../graph3d.h"
#include "util.h"

#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkPicker.h>
#include <vtkPointPicker.h>
#include <vtkPlane.h>
#include <vtkWorldPointPicker.h>
#include <vtkPropPicker.h>
#include <vtkCellPicker.h>
#include <vtkProp.h>

#include <QDebug>
#include <QApplication>

using namespace ComDataGui3D;

static const int HoverWidth = 7;
static const int NormalWidth = 5;
static const QColor PlaneHighlightColor = QColor(87, 87, 87);
static const QColor PlaneNormalColor = Qt::white;
static const QColor StickColor = QColor(233, 233, 233);
static const QColor HighlightStickColor = QColor(255, 0, 0);

vtkStandardNewMacro(DisableRotationInteractorStyle);
DisableRotationInteractorStyle::DisableRotationInteractorStyle()
{
    ComDataGui3D::detail::ArrayInitializer<double>()(m_topPoint);
    ComDataGui3D::detail::ArrayInitializer<double>()(m_bottomPoint);
}

DisableRotationInteractorStyle::~DisableRotationInteractorStyle()
{

}

void DisableRotationInteractorStyle::OnLeftButtonUp()
{
    CustomInteractorStyle::OnLeftButtonUp();

    notifySpherePosChanged();
    m_pickedActor = nullptr;
    resetActors();
    resetPickingCursor();

    if (m_isDirty) {
        m_isDirty = false;
        emit update();
    }
}

void DisableRotationInteractorStyle::OnMouseMove()
{
    CustomInteractorStyle::OnMouseMove();

    int x = m_movePos.x();
    int y = m_graph->height() - m_movePos.y();

    if (!m_pickedActor) {
        highlightActor();
    } else {
        m_isDirty = true;
        setPickingCursor();
        CREATE_VTKOBJ(vtkPointPicker, picker);
        picker->Pick(x, y, 0, GetDefaultRenderer());
        double* pickPos = picker->GetPickPosition();
        if (m_pickedActor == m_planeActor || m_pickedActor == m_frameActor) {
            emit planeChanged(computeBoundedPos(pickPos));
        } else if (m_pickedActor == m_originActor) {
            highlightOrigin(true);
            emit originChanged(pickPos);
        } else if (m_pickedActor == m_topBallActor ||
                   m_pickedActor == m_bottomBallActor ||
                   m_pickedActor == m_stickActor) {
            double topBallDis = ComDataGui3D::detail::distance(m_topPoint, pickPos);
            double bottomBallDis = ComDataGui3D::detail::distance(m_bottomPoint, pickPos);
            bool nearTopBall = topBallDis <= bottomBallDis;
            emit normalChanged(nearTopBall, pickPos);
        }
    }
}

void DisableRotationInteractorStyle::setCutType(Cutter::CutType ct)
{
    m_cutType = ct;
}

void DisableRotationInteractorStyle::setSphereActor(vtkActor *sphere)
{
    m_sphereActor = sphere;
}

void DisableRotationInteractorStyle::setPlaneActor(vtkActor* plane)
{
    m_planeActor = plane;
}

void DisableRotationInteractorStyle::setFrameActor(vtkActor* frame)
{
    m_frameActor = frame;
}

void DisableRotationInteractorStyle::setOriginActor(vtkActor* origin)
{
    m_originActor = origin;
}

void DisableRotationInteractorStyle::setTopBallActor(vtkActor* topBall)
{
    m_topBallActor = topBall;
}

void DisableRotationInteractorStyle::setBottomBallActor(vtkActor* bottomBall)
{
    m_bottomBallActor = bottomBall;
}

void DisableRotationInteractorStyle::setStickActor(vtkActor *stick)
{
    m_stickActor = stick;
}

void DisableRotationInteractorStyle::setOrigin(double* origin)
{
    ComDataGui3D::detail::ArrayAssigner<double>()(m_origin, origin);
}

void DisableRotationInteractorStyle::setNormal(double* normal)
{
    ComDataGui3D::detail::ArrayAssigner<double>()(m_normal, normal);
}

void DisableRotationInteractorStyle::onStickPointsChanged(double *topPot, double *bottomPot)
{
    ComDataGui3D::detail::ArrayAssigner<double>()(m_topPoint, topPot);
    ComDataGui3D::detail::ArrayAssigner<double>()(m_bottomPoint, bottomPot);
}

void DisableRotationInteractorStyle::resetActors()
{
    m_frameActor->GetProperty()->SetLineWidth(NormalWidth);
    double* clrArr = ComDataGui3D::detail::Util::vtkColor(PlaneNormalColor);
    m_planeActor->GetProperty()->SetColor(clrArr);
    highlightStick(false);
    m_graph->update();
}

void DisableRotationInteractorStyle::highlightStick(bool highlight)
{
    double* clrArr = nullptr;
    if (highlight)
        clrArr = ComDataGui3D::detail::Util::vtkColor(HighlightStickColor);
    else
        clrArr = ComDataGui3D::detail::Util::vtkColor(StickColor);

    m_stickActor->GetProperty()->SetColor(clrArr);
    m_topBallActor->GetProperty()->SetColor(clrArr);
    m_bottomBallActor->GetProperty()->SetColor(clrArr);
    m_originActor->GetProperty()->SetColor(clrArr);
    m_graph->update();
}

void DisableRotationInteractorStyle::highlightOrigin(bool highlight)
{
    double* clrArr = nullptr;
    if (highlight)
        clrArr = ComDataGui3D::detail::Util::vtkColor(HighlightStickColor);
    else
        clrArr = ComDataGui3D::detail::Util::vtkColor(StickColor);

    m_originActor->GetProperty()->SetColor(clrArr);
    m_graph->update();
}

void DisableRotationInteractorStyle::highlightActor()
{
    vtkActor* hoverdActor = this->hoverdActor();

    if (hoverdActor == m_planeActor) {
        setPickingCursor();
        m_planeActor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(PlaneHighlightColor));
        m_graph->update();
    } else if (hoverdActor == m_frameActor) {
        setPickingCursor();
        m_planeActor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(PlaneHighlightColor));
        m_graph->update();
    } else if (hoverdActor == m_originActor) {
        setPickingCursor();
        highlightOrigin(true);
    } else if (hoverdActor == m_stickActor ||
               hoverdActor == m_topBallActor ||
               hoverdActor == m_bottomBallActor){
        setPickingCursor();
        highlightStick(true);
    } else if (hoverdActor == m_sphereActor && m_sphereActor && m_sphereActor->GetVisibility()){
        setPickingCursor();
    } else {
        resetActors();
        resetPickingCursor();
    }
}

void DisableRotationInteractorStyle::notifySpherePosChanged()
{
    if (m_sphereActor && m_pickedActor == m_sphereActor && m_sphereActor->GetVisibility()) {
        int x = m_movePos.x();
        int y = m_graph->height() - m_movePos.y();

        CREATE_VTKOBJ(vtkPointPicker, picker);
        picker->Pick(x, y, 0, GetDefaultRenderer());
        double* pickPos = picker->GetPickPosition();
        emit originChanged(pickPos);
    }
}
