#include "planecutter.h"
#include "../graph3d.h"
#include "util.h"
#include "disablerotationinteractorstyle.h"
#include "mousemonitor.h"

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkCutter.h>
#include <vtkPolygon.h>
#include <vtkCellArray.h>
#include <vtkDelaunay2D.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkConeSource.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

static const QColor NormalStickColor = QColor(233, 233, 233);
static const QColor HighlightStickColor = QColor(255, 0, 0);
static const int StandardLineWidth = 5;
static const int StickLineWidth = 3;

class PlaneCutterPrivate
{
public:
    ~PlaneCutterPrivate();

    vtkActor* cutLineActor = nullptr;
    vtkActor* cutSurfaceActor = nullptr;
    vtkActor* originActor = nullptr;
    vtkActor* normalLineActor = nullptr;
    vtkActor* topBallActor = nullptr;
    vtkActor* bottomBallActor = nullptr;
    vtkPolygon* planePolygon = nullptr;

    double topBallPos[3];
    double bottomBallPos[3];

    bool isRotating = false;
    bool isChangingOrigin = false;
    bool isCutPlaneVisible = true;
    bool isArrowVisible = true;

    double origin[3];
    double normal[3];
    double plane[3];

    vtkPolyData* boxData = nullptr;
};

PlaneCutterPrivate::~PlaneCutterPrivate()
{
    ComDataGui3D::detail::vtkSafeDelete(planePolygon);
}

PlaneCutter::PlaneCutter(QObject* parent) : Interactor(parent)
{
    d_ptr = new PlaneCutterPrivate;
    d_ptr->planePolygon = vtkPolygon::New();
}

PlaneCutter::~PlaneCutter()
{
    delete d_ptr;
}

int PlaneCutter::type() const
{
    return PLANECUTTER;
}

void PlaneCutter::update()
{
    double projectPlaneNormal[3];
    ComDataGui3D::detail::Normalizer()(normal(), projectPlaneNormal);

    CREATE_VTKOBJ(vtkPlane, plane);
    plane->SetOrigin(this->origin());
    plane->SetNormal(projectPlaneNormal);

    CREATE_VTKOBJ(vtkCutter, clip);
    clip->SetInputData(d_ptr->boxData);
    clip->SetCutFunction(plane);
    clip->Update();

    CREATE_VTKOBJ(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(clip->GetOutputPort());

    ComDataGui3D::vtkInitOnce(&d_ptr->cutLineActor);
    d_ptr->cutLineActor->GetProperty()->SetLineWidth(StandardLineWidth);
    d_ptr->cutLineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    renderer()->AddActor(d_ptr->cutLineActor);
    d_ptr->cutLineActor->SetMapper(mapper);
    d_ptr->cutLineActor->SetVisibility(1);
    d_ptr->cutLineActor->SetPickable(1);

    vtkPolyData* clipOutput = clip->GetOutput();
    CREATE_VTKOBJ(vtkPolyData, pd);
    pd->DeepCopy(clipOutput);

    // origin
    double* bds = this->bounds();
    double xScale[2];
    double yScale[2];
    double zScale[2];
    xScale[0] = bds[0];
    xScale[1] = bds[1];
    yScale[0] = bds[2];
    yScale[1] = bds[3];
    zScale[0] = bds[4];
    zScale[1] = bds[5];

    double minScale = qMin((xScale[1] - xScale[0]), qMin((yScale[1] - yScale[0]), (zScale[1] - zScale[0])));

    int numOfPoints = clipOutput->GetNumberOfPoints();

    // create plane using points
    CREATE_VTKOBJ(vtkPolygon, polygon);
    polygon->GetPointIds()->SetNumberOfIds(numOfPoints); //make a quad

    CREATE_VTKOBJ(vtkCellArray, polygons);
    if (numOfPoints == 3) {
        vtkIdType p[3] = {0, 1, 2};
        polygons->InsertNextCell(numOfPoints, p);
    } else if (numOfPoints == 4) {
        vtkIdType p[4] = {0, 1, 2, 3};
        polygons->InsertNextCell(numOfPoints, p);
    } else if (numOfPoints == 5){
        vtkIdType p[5] = {0, 1, 2, 3, 4};
        polygons->InsertNextCell(numOfPoints, p);
    } else if (numOfPoints == 6) {
        vtkIdType p[6] = {0, 1, 2, 3, 4, 5};
        polygons->InsertNextCell(numOfPoints, p);
    }

    d_ptr->planePolygon->DeepCopy(polygon);

    // Add the polygon to a list of polygons
    pd->SetPolys(polygons);

    CREATE_VTKOBJ(vtkDelaunay2D, del);
    del->SetInputData(pd);
    del->Update();

    CREATE_VTKOBJ(vtkPolyDataMapper, surfaceMapper);
    surfaceMapper->SetInputData(del->GetOutput());
    surfaceMapper->Update();

    ComDataGui3D::vtkInitOnce(&d_ptr->cutSurfaceActor);
    d_ptr->cutSurfaceActor->SetMapper(surfaceMapper);
    d_ptr->cutSurfaceActor->GetProperty()->SetOpacity(0.4);
    d_ptr->cutSurfaceActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
    d_ptr->cutSurfaceActor->SetPickable(1);
    d_ptr->cutSurfaceActor->SetVisibility(d_ptr->isCutPlaneVisible);
    renderer()->AddActor(d_ptr->cutSurfaceActor);

    // render stick
    double *stickColorArr = ComDataGui3D::detail::Util::vtkColor(NormalStickColor);
    double ballRadius = minScale / 40;
    CREATE_VTKOBJ(vtkSphereSource, sphereSrc);
    sphereSrc->SetCenter(origin());
    sphereSrc->SetRadius(ballRadius);
    sphereSrc->Update();

    CREATE_VTKOBJ(vtkPolyDataMapper, sphereMapper);
    sphereMapper->SetInputData(sphereSrc->GetOutput());
    sphereMapper->Update();

    ComDataGui3D::vtkInitOnce(&d_ptr->originActor);
    d_ptr->originActor->SetMapper(sphereMapper);
    d_ptr->originActor->GetProperty()->SetColor(stickColorArr);
    renderer()->AddActor(d_ptr->originActor);
    d_ptr->originActor->SetVisibility(1);
    d_ptr->originActor->SetPickable(1);

    if (d_ptr->isChangingOrigin || d_ptr->isRotating)
        d_ptr->originActor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(HighlightStickColor));

    double topPot[3];
    double bottomPot[3];
    double halfLength = minScale * 0.65;

    // compute top & bottom points
    CREATE_VTKOBJ(vtkPlane, topPlane);
    topPlane->SetOrigin(this->origin());
    topPlane->SetNormal(projectPlaneNormal);
    topPlane->Push(halfLength);
    topPlane->ProjectPoint(this->origin(), topPot);

    CREATE_VTKOBJ(vtkPlane, bottomPlane);
    bottomPlane->SetOrigin(this->origin());
    bottomPlane->SetNormal(projectPlaneNormal);
    bottomPlane->Push(-halfLength);
    bottomPlane->ProjectPoint(this->origin(), bottomPot);

    CREATE_VTKOBJ(vtkLineSource, lineSrc);
    lineSrc->SetPoint1(topPot);
    lineSrc->SetPoint2(bottomPot);
    lineSrc->Update();

    CREATE_VTKOBJ(vtkPolyDataMapper, lineMapper);
    lineMapper->SetInputConnection(lineSrc->GetOutputPort());

    ComDataGui3D::vtkInitOnce(&d_ptr->normalLineActor);
    d_ptr->normalLineActor->SetMapper(lineMapper);
    d_ptr->normalLineActor->GetProperty()->SetColor(stickColorArr);
    d_ptr->normalLineActor->GetProperty()->SetLineWidth(StickLineWidth);
    renderer()->AddActor(d_ptr->normalLineActor);
    d_ptr->normalLineActor->SetVisibility(1);
    d_ptr->normalLineActor->SetPickable(1);

    // top ball
    CREATE_VTKOBJ(vtkConeSource, topConeSrc);
    topConeSrc->SetCenter(topPot);
    topConeSrc->SetDirection(normal());
    topConeSrc->SetResolution(20);
    topConeSrc->SetHeight(ballRadius * 3);
    topConeSrc->SetRadius(ballRadius);
    topConeSrc->Update();

    d_ptr->topBallPos[0] = topPot[0];
    d_ptr->topBallPos[1] = topPot[1];
    d_ptr->topBallPos[2] = topPot[2];

    CREATE_VTKOBJ(vtkPolyDataMapper, topSphereMapper);
    topSphereMapper->SetInputConnection(topConeSrc->GetOutputPort());
    ComDataGui3D::vtkInitOnce(&d_ptr->topBallActor);
    d_ptr->topBallActor->GetProperty()->SetColor(stickColorArr);
    d_ptr->topBallActor->SetMapper(topSphereMapper);
    d_ptr->topBallActor->SetVisibility(1);
    d_ptr->topBallActor->SetPickable(1);
    renderer()->AddActor(d_ptr->topBallActor);

    // bottom ball
    CREATE_VTKOBJ(vtkConeSource, bottomConeSrc);
    bottomConeSrc->SetCenter(bottomPot);
    bottomConeSrc->SetDirection(normal());
    bottomConeSrc->SetResolution(20);
    bottomConeSrc->SetHeight(ballRadius * 3);
    bottomConeSrc->SetRadius(ballRadius);
    bottomConeSrc->Update();

    d_ptr->bottomBallPos[0] = bottomPot[0];
    d_ptr->bottomBallPos[1] = bottomPot[1];
    d_ptr->bottomBallPos[2] = bottomPot[2];

    emit stickPointsChanged(topPot, bottomPot);

    CREATE_VTKOBJ(vtkPolyDataMapper, bottomSphereMapper);
    bottomSphereMapper->SetInputConnection(bottomConeSrc->GetOutputPort());
    ComDataGui3D::vtkInitOnce(&d_ptr->bottomBallActor);
    d_ptr->bottomBallActor->GetProperty()->SetColor(stickColorArr);
    d_ptr->bottomBallActor->SetMapper(bottomSphereMapper);
    d_ptr->bottomBallActor->SetVisibility(1);
    d_ptr->bottomBallActor->SetPickable(1);
    renderer()->AddActor(d_ptr->bottomBallActor);

    if (d_ptr->isRotating) {
        d_ptr->normalLineActor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(HighlightStickColor));
        d_ptr->topBallActor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(HighlightStickColor));
        d_ptr->bottomBallActor->GetProperty()->SetColor(ComDataGui3D::detail::Util::vtkColor(HighlightStickColor));
    }

    graph()->update();
}

void PlaneCutter::attach(ComDataGui3D::PostProcessor* pp)
{
    if (!pp)
        return;

    qDebug() << __FUNCTION__;

    Interactor::attach(pp);

    update();

    MouseMonitor* mm = new MouseMonitor(graph(), this);
    graph()->installEventFilter(mm);
    connect(mm, SIGNAL(mouseReleased()), this, SLOT(onMouseReleased()));

    // show plane actor
    setSpecialInteractorStyle();
}

void PlaneCutter::detach()
{
    renderer()->RemoveActor(d_ptr->cutLineActor);
    renderer()->RemoveActor(d_ptr->cutSurfaceActor);
    renderer()->RemoveActor(d_ptr->originActor);
    renderer()->RemoveActor(d_ptr->normalLineActor);
    renderer()->RemoveActor(d_ptr->topBallActor);
    renderer()->RemoveActor(d_ptr->bottomBallActor);
    Interactor::detach();
}

void PlaneCutter::setOrigin(double* origin, bool update)
{
    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->origin, origin);
    if (update)
        this->update();
}

double* PlaneCutter::origin() const
{
    return d_ptr->origin;
}

void PlaneCutter::setNormal(double* normal, bool update)
{
    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->normal, normal);
    if (update)
        this->update();
}

double* PlaneCutter::normal() const
{
    return d_ptr->normal;
}

void PlaneCutter::setBoxData(vtkPolyData *data)
{
    d_ptr->boxData = data;
}

void PlaneCutter::setArrowVisible(bool visible)
{
    if (d_ptr->topBallActor) {
        d_ptr->isArrowVisible = visible;
        d_ptr->topBallActor->SetVisibility(visible);
        d_ptr->bottomBallActor->SetVisibility(visible);
        d_ptr->normalLineActor->SetVisibility(visible);
        d_ptr->originActor->SetVisibility(visible);
        graph()->update();
    }
}

bool PlaneCutter::isArrowVisible() const
{
    return d_ptr->isArrowVisible;
}

void PlaneCutter::setCutPlaneVisible(bool visible)
{
    if (d_ptr->cutSurfaceActor) {
        d_ptr->isCutPlaneVisible = visible;
        d_ptr->cutSurfaceActor->SetVisibility(visible);
        graph()->update();
    }
}

bool PlaneCutter::isCutPlaneVisible() const
{
    if (d_ptr->cutSurfaceActor)
        return d_ptr->cutSurfaceActor->GetVisibility();
    return false;
}

void PlaneCutter::setVisible(bool visible)
{
    if (d_ptr->cutLineActor && d_ptr->cutLineActor->GetVisibility() != visible) {
        d_ptr->cutLineActor->SetVisibility(visible);
        d_ptr->cutSurfaceActor->SetVisibility(visible);
        d_ptr->originActor->SetVisibility(visible);
        d_ptr->normalLineActor->SetVisibility(visible);
        d_ptr->topBallActor->SetVisibility(visible);
        d_ptr->bottomBallActor->SetVisibility(visible);

        Interactor::setVisible(visible);
    }
}

bool PlaneCutter::isVisible() const
{
    if (d_ptr->cutLineActor)
        return d_ptr->cutLineActor->GetVisibility();
    return Interactor::isVisible();
}

void PlaneCutter::onMouseReleased()
{
    d_ptr->isChangingOrigin = false;
    d_ptr->isRotating = false;
}

void PlaneCutter::onUpdate()
{
    setArrowVisible(true);
    processor()->apply();
}

void PlaneCutter::onOriginChanged(double *pos)
{
    d_ptr->isChangingOrigin = true;
    double* bds = bounds();
    double tmp[3];
    tmp[0] = ComDataGui3D::detail::boundedValue(pos[0], bds[0], bds[1]);
    tmp[1] = ComDataGui3D::detail::boundedValue(pos[1], bds[2], bds[3]);
    tmp[2] = ComDataGui3D::detail::boundedValue(pos[2], bds[4], bds[5]);

    double newOrigin[3];
    vtkPlane::ProjectPoint(tmp, this->origin(), this->normal(), newOrigin);
    setOrigin(newOrigin);
    emit originChanged(newOrigin);
}

void PlaneCutter::onNormalChanged(bool positive, double *pos)
{
    d_ptr->isRotating = true;
    double newNormal[3];

    if (positive) {
        newNormal[0] = pos[0] - this->origin()[0];
        newNormal[1] = pos[1] - this->origin()[1];
        newNormal[2] = pos[2] - this->origin()[2];
    } else {
        newNormal[0] = this->origin()[0] - pos[0];
        newNormal[1] = this->origin()[1] - pos[1];
        newNormal[2] = this->origin()[2] - pos[2];
    }

    ComDataGui3D::detail::Normalizer()(newNormal, newNormal);
    setNormal(newNormal);

    emit normalChanged(newNormal);
}

void PlaneCutter::onPlaneChanged(double *pos)
{
    double newOri[3];

    // have to use normalized normal
    double newNormal[3];
    ComDataGui3D::detail::Normalizer()(normal(), newNormal);
    vtkPlane::ProjectPoint(origin(), pos, newNormal, newOri);

    double* bds = bounds();
    newOri[0] = ComDataGui3D::detail::boundedValue(newOri[0], bds[0], bds[1]);
    newOri[1] = ComDataGui3D::detail::boundedValue(newOri[1], bds[2], bds[3]);
    newOri[2] = ComDataGui3D::detail::boundedValue(newOri[2], bds[4], bds[5]);

    setOrigin(newOri);

    emit originChanged(newOri);
}

void PlaneCutter::setSpecialInteractorStyle()
{
    DisableRotationInteractorStyle* style = DisableRotationInteractorStyle::New();

    style->setGraph(graph());
    style->SetDefaultRenderer(renderer());
    style->setPlaneActor(d_ptr->cutSurfaceActor);
    style->setFrameActor(d_ptr->cutLineActor);
    style->setOriginActor(d_ptr->originActor);
    style->setTopBallActor(d_ptr->topBallActor);
    style->setBottomBallActor(d_ptr->bottomBallActor);
    style->setStickActor(d_ptr->normalLineActor);
    style->setOrigin(origin());
    style->setNormal(normal());
    style->setBounds(bounds());

    // in the initial state, set the top & bottom ball's position
    style->onStickPointsChanged(d_ptr->topBallPos, d_ptr->bottomBallPos);

    connect(this, SIGNAL(stickPointsChanged(double*,double*)), style, SLOT(onStickPointsChanged(double*,double*)));
    connect(style, SIGNAL(originChanged(double*)), this, SLOT(onOriginChanged(double*)));
    connect(style, SIGNAL(normalChanged(bool, double*)), this, SLOT(onNormalChanged(bool, double*)));
    connect(style, SIGNAL(planeChanged(double*)), this, SLOT(onPlaneChanged(double*)));
    connect(this, SIGNAL(originChanged(double*)), style, SLOT(setOrigin(double*)));
    connect(graph(), SIGNAL(clickPositionChanged(QPoint)), style, SLOT(setClickPosition(QPoint)));
    connect(graph(), SIGNAL(movePositionChanged(QPoint)), style, SLOT(setMovePosition(QPoint)));
    connect(style, SIGNAL(update()), this, SLOT(onUpdate()));

    graph()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
}
