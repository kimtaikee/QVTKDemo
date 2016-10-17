#include "pointmover.h"
#include "util.h"
#include "movepointinteractorstyle.h"
#include "../graph3d.h"
#include "../postprocessor.h"

#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

class PointMoverPrivate
{
public:
    vtkActor* pointActor = nullptr;
    double position[3];
};

PointMover::PointMover(QObject* parent) : Interactor(parent)
{
    d_ptr = new PointMoverPrivate;
    ComDataGui3D::detail::ArrayInitializer<double>()(d_ptr->position);
}

PointMover::~PointMover()
{
    delete d_ptr;
}

int PointMover::type() const
{
    return POINTMOVER;
}

void PointMover::update()
{
    double* bds = this->bounds();
    ComDataGui3D::vtkInitOnce(&d_ptr->pointActor);
    d_ptr->pointActor->SetPickable(true);

    double xRange = bds[1] - bds[0];
    double yRange = bds[3] - bds[2];
    double zRange = bds[5] - bds[4];

    double maxRange = qMax(qMax(xRange, yRange), zRange);
    double radius = maxRange / 100;

    CREATE_VTKOBJ(vtkSphereSource, ss);
    ss->SetRadius(radius);
    ss->SetCenter(d_ptr->position);
    ss->Update();

    CREATE_VTKOBJ(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(ss->GetOutputPort());

    d_ptr->pointActor->SetMapper(mapper);
    renderer()->AddActor(d_ptr->pointActor);
    graph()->update();
}

void PointMover::attach(ComDataGui3D::PostProcessor* pp)
{
    if (!pp)
        return;

    Interactor::attach(pp);
    update();
    setSpecialInteractorStyle();
}

void PointMover::detach()
{
    renderer()->RemoveActor(d_ptr->pointActor);
    Interactor::detach();
}

void PointMover::setPosition(double* pos)
{
    if (ComDataGui3D::detail::ArrayComparator<double>()(d_ptr->position, pos))
        return;

    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->position, pos);
    update();
}

double* PointMover::position() const
{
    return d_ptr->position;
}

void PointMover::setPointVisible(bool visible)
{
    if (d_ptr->pointActor)
        d_ptr->pointActor->SetVisibility(visible);
}

bool PointMover::isPointVisible() const
{
    if (d_ptr->pointActor)
        return d_ptr->pointActor->GetVisibility();
    return false;
}

void PointMover::setVisible(bool visible)
{
    if (d_ptr->pointActor && d_ptr->pointActor->GetVisibility() != visible) {
        d_ptr->pointActor->SetVisibility(visible);
        Interactor::setVisible(visible);
    }
}

bool PointMover::isVisible() const
{
    return isPointVisible();
}

void PointMover::onCenterChanged(double *center)
{
    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->position, center);
    update();
}

void PointMover::onMouseReleased()
{
    if (processor())
        processor()->apply();
}

void PointMover::setSpecialInteractorStyle()
{
    MovePointInteractorStyle* style = MovePointInteractorStyle::New();
    style->setGraph(graph());
    style->SetDefaultRenderer(renderer());
    style->setBounds(bounds());
    style->setActor(d_ptr->pointActor);

    connect(graph(), SIGNAL(clickPositionChanged(QPoint)), style, SLOT(setClickPosition(QPoint)), Qt::DirectConnection);
    connect(graph(), SIGNAL(movePositionChanged(QPoint)), style, SLOT(setMovePosition(QPoint)), Qt::DirectConnection);

    connect(style, SIGNAL(centerChanged(double*)), this, SIGNAL(pointChanged(double*)));
    connect(style, SIGNAL(centerChanged(double*)), this, SLOT(onCenterChanged(double*)));
    connect(style, SIGNAL(mouseReleased()), this, SLOT(onMouseReleased()), Qt::DirectConnection);
    graph()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
}
