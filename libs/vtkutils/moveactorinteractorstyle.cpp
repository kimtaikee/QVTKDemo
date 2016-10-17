#include "moveactorinteractorstyle.h"

#include <utils/utils.h>

#include <vtkutils/vtkutils.h>

#include <vtkObjectFactory.h>
#include <vtkPickingManager.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPicker.h>
#include <vtkCellPicker.h>
#include <vtkPropPicker.h>
#include <vtkPointPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkExtractEdges.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <QDebug>

static const QColor HighlightColor = Qt::yellow;

namespace VtkUtils
{

vtkStandardNewMacro(MoveActorInteractorStyle);
MoveActorInteractorStyle::MoveActorInteractorStyle(QObject *parent) : QObject(parent)
{

}

void MoveActorInteractorStyle::OnLeftButtonDown()
{
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    int x = GetInteractor()->GetEventPosition()[0];
    int y = GetInteractor()->GetEventPosition()[1];

    VTK_CREATE(vtkPicker, picker);
    picker->SetTolerance(0.001);
    vtkRenderer* defaultRenderer = GetDefaultRenderer();
    picker->Pick(x, y, 0, defaultRenderer);
    m_pickedActor = picker->GetActor();
}

void MoveActorInteractorStyle::OnLeftButtonUp()
{
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    emit actorMoved(m_pickedActor);
    m_pickedActor = nullptr;
}

void MoveActorInteractorStyle::Rotate()
{
    if (m_pickedActor)
        return;

    vtkInteractorStyleTrackballCamera::Rotate();
}

void MoveActorInteractorStyle::Spin()
{
    if (m_pickedActor)
        return;

    vtkInteractorStyleTrackballCamera::Spin();
}

void MoveActorInteractorStyle::OnMouseMove()
{
    vtkInteractorStyleTrackballCamera::OnMouseMove();

    if (m_useHighlight) {
        int x = GetInteractor()->GetEventPosition()[0];
        int y = GetInteractor()->GetEventPosition()[1];

        VTK_CREATE(vtkPicker, picker);
        picker->SetTolerance(0.001);
        vtkRenderer* defaultRenderer = GetDefaultRenderer();
        picker->Pick(x, y, 0, defaultRenderer);
        vtkActor* hoverActor = picker->GetActor();

        if (hoverActor) {
            vtkDataSet* dataSet = hoverActor->GetMapper()->GetInput();
            double* hoverPos = picker->GetPickPosition();

            VtkUtils::vtkInitOnce(m_highlightActor);
            m_highlightActor->SetPickable(false);
            VTK_CREATE(vtkExtractEdges, edges);
            edges->SetInputData(dataSet);

            VTK_CREATE(vtkPolyDataMapper, mapper);
            mapper->SetInputConnection(edges->GetOutputPort());

            double clrArr[3];
            Utils::vtkColor(HighlightColor, clrArr);

            m_highlightActor->SetMapper(mapper);
            m_highlightActor->SetVisibility(1);
            m_highlightActor->GetProperty()->SetColor(clrArr);
            m_highlightActor->SetPosition(hoverPos);
            defaultRenderer->AddActor(m_highlightActor);
            defaultRenderer->GetRenderWindow()->Render();
        } else {
            if (m_highlightActor) {
                m_highlightActor->SetVisibility(0);
                defaultRenderer->GetRenderWindow()->Render();
            }
        }
    }

    if (m_pickedActor) {
        int x = GetInteractor()->GetEventPosition()[0];
        int y = GetInteractor()->GetEventPosition()[1];

        VTK_CREATE(vtkPicker, picker);
        picker->SetTolerance(0.001);
        vtkRenderer* defaultRenderer = GetDefaultRenderer();
        picker->Pick(x, y, 0, defaultRenderer);

        double* movePos = picker->GetPickPosition();
        m_pickedActor->SetPosition(movePos);
        defaultRenderer->GetRenderWindow()->Render();
    }

}

void MoveActorInteractorStyle::OnChar()
{
    vtkInteractorStyleTrackballCamera::OnChar();
}

void MoveActorInteractorStyle::setHighlightActor(bool highlight)
{
    m_useHighlight = highlight;
}

bool MoveActorInteractorStyle::highlightActor() const
{
    return m_useHighlight;
}


} // namespace VtkUtils
