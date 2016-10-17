#include "spherecutter.h"
#include "util.h"
#include "../graph3d.h"
#include "../postprocessor.h"
#include "movepointinteractorstyle.h"

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>

class SphereCutterPrivate
{
public:
    SphereCutterPrivate();
    ~SphereCutterPrivate();

    double center[3];
    double radius = 1.0;

    vtkSphereSource* sphereSrc = nullptr;
    vtkPolyDataMapper* mapper = nullptr;
    vtkActor* actor = nullptr;
};

SphereCutterPrivate::SphereCutterPrivate()
{

}

SphereCutterPrivate::~SphereCutterPrivate()
{
    ComDataGui3D::detail::vtkSafeDelete(actor);
}

SphereCutter::SphereCutter(QObject* parent) : Interactor(parent)
{
    d_ptr = new SphereCutterPrivate;
}

SphereCutter::~SphereCutter()
{
    delete d_ptr;
}

void SphereCutter::setCenter(double* pot, bool update)
{
    if (ComDataGui3D::detail::ArrayComparator<double>()(d_ptr->center, pot))
        return;

    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->center, pot);
    if (update)
        this->update();
}

double *SphereCutter::center() const
{
    return d_ptr->center;
}

void SphereCutter::setRadius(double radius, bool update)
{
    if (d_ptr->radius != radius) {
        d_ptr->radius = radius;
        if (update)
            this->update();
    }
}

double SphereCutter::radius() const
{
    return d_ptr->radius;
}

int SphereCutter::type() const
{
    return SPHERECUTTER;
}

void SphereCutter::update()
{
    ComDataGui3D::vtkInitOnce(&d_ptr->sphereSrc);
    d_ptr->sphereSrc->SetCenter(d_ptr->center);
    d_ptr->sphereSrc->SetRadius(d_ptr->radius);
    d_ptr->sphereSrc->Update();

    ComDataGui3D::vtkInitOnce(&d_ptr->mapper);
    d_ptr->mapper->SetInputConnection(d_ptr->sphereSrc->GetOutputPort());

    ComDataGui3D::vtkInitOnce(&d_ptr->actor);
    d_ptr->actor->SetMapper(d_ptr->mapper);
    d_ptr->actor->GetProperty()->SetRepresentationToWireframe();

    renderer()->AddActor(d_ptr->actor);
    graph()->update();
}

void SphereCutter::attach(ComDataGui3D::PostProcessor* pp)
{
    if (!pp)
        return;

    qDebug() << __FUNCTION__;
    Interactor::attach(pp);

    double* bds = this->bounds();

    d_ptr->center[0] = bds[0] + (bds[1] - bds[0]) / 2;
    d_ptr->center[1] = bds[2] + (bds[3] - bds[2]) / 2;
    d_ptr->center[2] = bds[4] + (bds[5] - bds[4]) / 2;

    update();
    setSpecialInteractorStyle();
}

void SphereCutter::detach()
{
    renderer()->RemoveActor(d_ptr->actor);
    Interactor::detach();
}

void SphereCutter::setVisible(bool visible)
{
    if (d_ptr->actor && d_ptr->actor->GetVisibility() != visible) {
        d_ptr->actor->SetVisibility(visible);
        Interactor::setVisible(visible);
    }
}

bool SphereCutter::isVisible() const
{
    if (d_ptr->actor)
        return d_ptr->actor->GetVisibility();
    return Interactor::isVisible();
}

void SphereCutter::onMouseReleased()
{
    if (processor())
        processor()->apply();
}

void SphereCutter::setSpecialInteractorStyle()
{
    MovePointInteractorStyle* style = MovePointInteractorStyle::New();
    style->setGraph(graph());
    style->SetDefaultRenderer(renderer());
    style->setBounds(bounds());
    style->setActor(d_ptr->actor);

    connect(graph(), SIGNAL(clickPositionChanged(QPoint)), style, SLOT(setClickPosition(QPoint)), Qt::DirectConnection);
    connect(graph(), SIGNAL(movePositionChanged(QPoint)), style, SLOT(setMovePosition(QPoint)), Qt::DirectConnection);
    connect(style, SIGNAL(centerChanged(double*)), this, SIGNAL(centerChanged(double*)));
    connect(style, SIGNAL(centerChanged(double*)), this, SLOT(setCenter(double*)));
    connect(style, SIGNAL(mouseReleased()), this, SLOT(onMouseReleased()));
    graph()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
}
