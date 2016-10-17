#include "selectareainteractorstyle.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkAreaPicker.h>
#include <vtkPlanes.h>

#include <QDebug>

vtkStandardNewMacro(SelectAreaInteractorStyle);
SelectAreaInteractorStyle::SelectAreaInteractorStyle(QObject *parent) : QObject(parent)
{
}

void SelectAreaInteractorStyle::OnLeftButtonUp()
{
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
    vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();
    emit areaSelected(frustum);
}

