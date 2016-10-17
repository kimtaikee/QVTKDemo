#include "vtkwidget.h"

#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>

#include <utils/utils.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/rendererslayoutalgo.h>

#include <QDebug>

namespace VtkUtils
{

class VtkWidgetPrivate
{
public:
    VtkWidgetPrivate(VtkWidget* q);
    ~VtkWidgetPrivate();

    void init();
    void configRenderer(vtkRenderer* renderer);
    void layoutRenderers();

    VtkWidget* q_ptr;
    QColor backgroundColor = Qt::black;
    bool multiViewports = false;
    vtkRenderer* defaultRenderer = nullptr;
    vtkOrientationMarkerWidget* orientationMarkerWidget = nullptr;

    QList<vtkRenderer*> renderers;
    QList<vtkProp*> actors;
    QList<vtkProp*> props;

    double bounds[6];
};

VtkWidgetPrivate::VtkWidgetPrivate(VtkWidget *q) : q_ptr(q)
{
    init();
}

VtkWidgetPrivate::~VtkWidgetPrivate()
{
}

void VtkWidgetPrivate::configRenderer(vtkRenderer *renderer)
{
    if (!renderer)
        return;

    double bgclr[3];
    Utils::vtkColor(backgroundColor, bgclr);

    renderer->SetBackground(bgclr);
}

static int columnCount(int count)
{
    int cols = 1;
    while (true) {
        if ((cols * cols) >= count)
            return cols;
        ++cols;
    }
    return cols;
}

void VtkWidgetPrivate::layoutRenderers()
{
    switch (renderers.size()) {
    case 1:
        VtkUtils::layoutRenderers<1>(renderers);
        break;

    case 2:
        VtkUtils::layoutRenderers<2>(renderers);
        break;

    case 3:
        VtkUtils::layoutRenderers<3>(renderers);
        break;

    case 4:
        VtkUtils::layoutRenderers<4>(renderers);
        break;

    case 5:
        VtkUtils::layoutRenderers<5>(renderers);
        break;

    case 6:
        VtkUtils::layoutRenderers<6>(renderers);
        break;

    case 7:
        VtkUtils::layoutRenderers<7>(renderers);
        break;

    case 8:
        VtkUtils::layoutRenderers<8>(renderers);
        break;

    case 9:
        VtkUtils::layoutRenderers<9>(renderers);
        break;

    case 10:
        VtkUtils::layoutRenderers<10>(renderers);
        break;

    default:
        VtkUtils::layoutRenderers<-1>(renderers);
    }
}

void VtkWidgetPrivate::init()
{
    layoutRenderers();
}

VtkWidget::VtkWidget(QWidget* parent) : QVTKWidget(parent)
{
    d_ptr = new VtkWidgetPrivate(this);
}

VtkWidget::~VtkWidget()
{
    delete d_ptr;
}

void VtkWidget::setMultiViewports(bool multi)
{
    if (d_ptr->multiViewports != multi) {
        d_ptr->multiViewports = multi;
    }
}

bool VtkWidget::multiViewports() const
{
    return d_ptr->multiViewports;
}

void VtkWidget::addActor(vtkProp* actor, const QColor& clr)
{
    if (!actor || d_ptr->actors.contains(actor))
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);

    d_ptr->actors.append(actor);

    if (!d_ptr->multiViewports) {
        if (d_ptr->renderers.isEmpty()) {
            vtkRenderer* renderer = vtkRenderer::New();
            renderer->SetBackground(vtkClr);
            d_ptr->configRenderer(renderer);
            renderer->AddActor(actor);
            GetRenderWindow()->AddRenderer(renderer);
            d_ptr->renderers.append(renderer);
            renderer->ResetCamera();
        } else {
            defaultRenderer()->SetBackground(vtkClr);
            defaultRenderer()->AddActor(actor);
        }
    } else {
        if (!defaultRendererTaken()) {
            defaultRenderer()->SetBackground(vtkClr);
            defaultRenderer()->AddActor(actor);
        } else {
            vtkRenderer* renderer = vtkRenderer::New();
            renderer->SetBackground(vtkClr);
            d_ptr->configRenderer(renderer);
            renderer->AddActor(actor);
            GetRenderWindow()->AddRenderer(renderer);
            d_ptr->renderers.append(renderer);
            d_ptr->layoutRenderers();
            renderer->ResetCamera();
        }
    }
}

void VtkWidget::addViewProp(vtkProp* prop)
{
    if (!prop || d_ptr->props.contains(prop))
        return;

    d_ptr->props.append(prop);

    if (!d_ptr->multiViewports) {
        if (d_ptr->renderers.isEmpty()) {
            vtkRenderer* renderer = vtkRenderer::New();
            d_ptr->configRenderer(renderer);
            renderer->AddViewProp(prop);
            GetRenderWindow()->AddRenderer(renderer);
            d_ptr->renderers.append(renderer);
            renderer->ResetCamera();
        } else {
            defaultRenderer()->AddViewProp(prop);
//            GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddViewProp(prop);
        }
    } else {

        if (!defaultRendererTaken()) {
            defaultRenderer()->AddViewProp(prop);
        } else {
            vtkRenderer* renderer = vtkRenderer::New();
            d_ptr->configRenderer(renderer);
            renderer->AddViewProp(prop);
            GetRenderWindow()->AddRenderer(renderer);
            d_ptr->renderers.append(renderer);
            d_ptr->layoutRenderers();
            renderer->ResetCamera();
        }
    }
}

QList<vtkProp*> VtkWidget::actors() const
{
    return d_ptr->actors;
}

void VtkWidget::setActorsVisible(bool visible)
{
    foreach (auto actor, d_ptr->actors)
        actor->SetVisibility(visible);
}

void VtkWidget::setActorVisible(vtkProp* actor, bool visible)
{
    actor->SetVisibility(visible);
}

bool VtkWidget::actorVisible(vtkProp* actor)
{
    return actor->GetVisibility();
}

void VtkWidget::setBackgroundColor(const QColor& clr)
{
    if (d_ptr->backgroundColor != clr) {
        d_ptr->backgroundColor = clr;

        foreach (vtkRenderer* renderer, d_ptr->renderers)
            d_ptr->configRenderer(renderer);

#if 0
        vtkRendererCollection* renderers = GetRenderWindow()->GetRenderers();
        vtkRenderer* renderer = renderers->GetFirstRenderer();
        while (renderer) {
            renderer = renderers->GetNextItem();
        }
#endif
        update();
    }
}

QColor VtkWidget::backgroundColor() const
{
    return d_ptr->backgroundColor;
}

vtkRenderer* VtkWidget::defaultRenderer()
{
    VtkUtils::vtkInitOnce(&d_ptr->defaultRenderer);
    GetRenderWindow()->AddRenderer(d_ptr->defaultRenderer);
    if (!d_ptr->renderers.contains(d_ptr->defaultRenderer))
        d_ptr->renderers.append(d_ptr->defaultRenderer);
    return d_ptr->defaultRenderer;
}

bool VtkWidget::defaultRendererTaken() const
{
    if (!d_ptr->defaultRenderer)
        return false;
    return d_ptr->defaultRenderer->GetActors()->GetNumberOfItems() != 0;
}

void VtkWidget::showOrientationMarker(bool show)
{
    if (!d_ptr->orientationMarkerWidget) {
        VTK_CREATE(vtkAxesActor, axes);
        axes->SetShaftTypeToCylinder();

        d_ptr->orientationMarkerWidget = vtkOrientationMarkerWidget::New();
        d_ptr->orientationMarkerWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
        d_ptr->orientationMarkerWidget->SetOrientationMarker(axes);
        d_ptr->orientationMarkerWidget->SetInteractor(GetInteractor());
        d_ptr->orientationMarkerWidget->SetViewport(0.0, 0.0, 0.23, 0.23);
        d_ptr->orientationMarkerWidget->SetEnabled(1);
        d_ptr->orientationMarkerWidget->InteractiveOff();
    }

    d_ptr->orientationMarkerWidget->SetEnabled(show);
    update();
}

void VtkWidget::setBounds(double* bounds)
{
    Utils::ArrayAssigner<double, 6> aa;
    aa(bounds, d_ptr->bounds);
}

double VtkWidget::xMin() const
{
    return d_ptr->bounds[0];
}

double VtkWidget::xMax() const
{
    return d_ptr->bounds[1];
}

double VtkWidget::yMin() const
{
    return d_ptr->bounds[2];
}

double VtkWidget::yMax() const
{
    return d_ptr->bounds[3];
}

double VtkWidget::zMin() const
{
    return d_ptr->bounds[4];
}

double VtkWidget::zMax() const
{
    return d_ptr->bounds[5];
}

} // namespace VtkUtils
