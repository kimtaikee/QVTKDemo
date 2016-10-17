#include "contour2dinteractorstyle.h"

#include <vtkutils/vtkutils.h>

#include <vtkPointPicker.h>
#include <vtkRenderWindowInteractor.h>

Contour2DInteractorStyle::Contour2DInteractorStyle(QObject* parent) : QObject(parent)
{

}

void Contour2DInteractorStyle::OnLeftButtonDown()
{
//    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    int x = GetInteractor()->GetEventPosition()[0];
    int y = GetInteractor()->GetEventPosition()[1];

    VTK_CREATE(vtkPicker, picker);
    picker->SetTolerance(0.001);
    vtkRenderer* defaultRenderer = GetDefaultRenderer();
    picker->Pick(x, y, 0, defaultRenderer);
    emit mousePressed(picker->GetPickPosition());
}
