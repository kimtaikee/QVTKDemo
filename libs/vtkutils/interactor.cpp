#include "interactor.h"
#include "../graph3d.h"
#include "../postprocessor.h"
#include "util.h"

#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

class InteractorPrivate
{
public:
    ComDataGui3D::Graph3D* graph = nullptr;
    ComDataGui3D::PostProcessor* postprocessor = nullptr;
    vtkRenderer* renderer = nullptr;
    bool isHooked = false;
    bool isVisible = false;
};

Interactor::Interactor(QObject* parent) : QObject(parent)
{
    d_ptr = new InteractorPrivate;
}

Interactor::~Interactor()
{
    delete d_ptr;
}

int Interactor::type() const
{
    return DEFAULT;
}

void Interactor::attach(ComDataGui3D::PostProcessor* pp)
{
    if (d_ptr->postprocessor != pp) {
        d_ptr->postprocessor = pp;
        d_ptr->graph = pp->graph();
        d_ptr->renderer = pp->renderer();
        d_ptr->isHooked = true;
    }
}

void Interactor::detach()
{
    Q_ASSERT(graph() != nullptr);

    setDefaultInteractorStyle();

    d_ptr->postprocessor = nullptr;
    d_ptr->graph = nullptr;
    d_ptr->renderer = nullptr;
    d_ptr->isHooked = false;
}

bool Interactor::isHooked() const
{
    return d_ptr->isHooked;
}

void Interactor::update()
{
    // no impl
    d_ptr->graph->update();
}

void Interactor::setVisible(bool visible)
{
    if (d_ptr->isVisible != visible) {
        if (visible)
            setSpecialInteractorStyle();
        else
            setDefaultInteractorStyle();
        graph()->update();
    }
}

bool Interactor::isVisible() const
{
    return d_ptr->isVisible;
}

ComDataGui3D::Graph3D* Interactor::graph() const
{
    return d_ptr->graph;
}

vtkRenderer* Interactor::renderer() const
{
    return d_ptr->renderer;
}

ComDataGui3D::PostProcessor* Interactor::processor() const
{
    return d_ptr->postprocessor;
}

double* Interactor::bounds() const
{
    if (d_ptr->postprocessor)
        return d_ptr->postprocessor->bounds();
    return nullptr;
}

void Interactor::setDefaultInteractorStyle()
{
    vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New();
    style->SetDefaultRenderer(renderer());
    graph()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
}

void Interactor::setSpecialInteractorStyle()
{

}
