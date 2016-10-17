#include "movelineinteractorstyle.h"
#include "../graph3d.h"
#include "util.h"

#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPointPicker.h>
#include <vtkPlane.h>
#include <vtkMath.h>

vtkStandardNewMacro(MoveLineInteractorStyle);
MoveLineInteractorStyle::MoveLineInteractorStyle()
{
}

MoveLineInteractorStyle::~MoveLineInteractorStyle()
{
}

void MoveLineInteractorStyle::OnMouseMove()
{
    CustomInteractorStyle::OnMouseMove();

    if (m_pickedActor) {

        int x = m_movePos.x();
        int y = m_graph->height() - m_movePos.y();

        setPickingCursor();

        CREATE_VTKOBJ(vtkPointPicker, picker);
        picker->Pick(x, y, 0, GetDefaultRenderer());
        double* pickPos = picker->GetPickPosition();

        if (m_pickedActor == m_firstBall) {
            emit point1Changed(computeBoundedPos(pickPos));
        } else if (m_pickedActor == m_secondBall){
            emit point2Changed(computeBoundedPos(pickPos));
        } else if (m_pickedActor == m_line) {

            return;
            // calculate the two ending points
            qDebug() << "click pos: " << m_clickPos;

            double origin[3];
            double normal[3];
            ComDataGui3D::Math::middlePoint(m_pot1, m_pot2, origin);
            normal[0] = m_pot2[0] - m_pot1[0];
            normal[1] = m_pot2[1] - m_pot1[1];
            normal[2] = m_pot2[2] - m_pot1[2];

            int clickX = m_clickPos.x();
            int clickY = m_graph->height() - m_clickPos.y();

            CREATE_VTKOBJ(vtkPointPicker, picker);
            picker->Pick(clickX, clickY, 0, GetDefaultRenderer());
            double* clickPos = picker->GetPickPosition();

            CREATE_VTKOBJ(vtkPlane, plane);
            plane->SetOrigin(clickPos);
            plane->SetNormal(normal);

            double dist = vtkMath::Distance2BetweenPoints(pickPos, clickPos);
            plane->Push(dist);
            qDebug() << "distance: " << dist;

            double firstPos[3];
            double secondPos[3];
            plane->ProjectPoint(m_pot1, firstPos);
            plane->ProjectPoint(m_pot2, secondPos);

            qDebug() << "firstPos: " << firstPos[0] << "," << firstPos[1] << "," << firstPos[2];
            qDebug() << "secondPos: " << secondPos[0] << "," << secondPos[1] << "," << secondPos[2];

            emit pointsChanged(computeBoundedPos(firstPos), computeBoundedPos(secondPos));
        }
    }
}

void MoveLineInteractorStyle::OnLeftButtonUp()
{
    CustomInteractorStyle::OnLeftButtonUp();
    resetPickingCursor();
}

void MoveLineInteractorStyle::setActors(vtkActor* firstBall, vtkActor* secondBall, vtkActor* line)
{
    m_firstBall = firstBall;
    m_secondBall = secondBall;
    m_line = line;

    addWatchActor(m_firstBall);
    addWatchActor(m_secondBall);
    addWatchActor(m_line);
}

void MoveLineInteractorStyle::setPoints(double *pot1, double *pot2)
{
    ComDataGui3D::detail::ArrayAssigner<double>()(m_pot1, pot1);
    ComDataGui3D::detail::ArrayAssigner<double>()(m_pot2, pot2);
}


