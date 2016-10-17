#include "pickcellinteractorstyle.h"

#include <vtkutils/vtkutils.h>

#include <vtkObjectFactory.h>
#include <vtkCellPicker.h>
#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(PickCellInteractorStyle);
PickCellInteractorStyle::PickCellInteractorStyle(QObject *parent) : QObject(parent)
{

}

void PickCellInteractorStyle::OnLeftButtonDown()
{
    int* pos = this->GetInteractor()->GetEventPosition();

    VTK_CREATE(vtkCellPicker, picker);
    picker->SetTolerance(0.0005);
    picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());
    emit cellPicked(picker->GetCellId());

    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}
