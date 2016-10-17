#include "lineprober.h"
#include "detail/util.h"
#include "qcustomplot.h"
#include "movelineinteractorstyle.h"
#include "../graph3d.h"
#include "../postprocessor.h"

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkXYPlotActor.h>
#include <vtkMath.h>
#include <vtkFieldData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include <QLabel>
#include <QEvent>
#include <QVTKWidget.h>
#include <QSplitter>

static const char *DefaultColors[] =
{
    "LightSalmon",
    "SteelBlue",
    "Yellow",
    "Fuchsia",
    "PaleGreen",
    "PaleTurquoise",
    "Cornsilk",
    "HotPink",
    "Peru",
    "Maroon"
};

class Plot2DOverlay;
class LineProberPrivate
{
public:
    LineProberPrivate();

    vtkActor* firstBallActor = nullptr;
    vtkActor* secondBallActor = nullptr;
    vtkActor* lineActor = nullptr;

    double pot1[3];
    double pot2[3];

    bool isPlotVisible = true;

    QCustomPlot* plot = nullptr;
};

LineProberPrivate::LineProberPrivate()
{
}

LineProber::LineProber(QObject* parent) : Interactor(parent)
{
    d_ptr = new LineProberPrivate;
}

int LineProber::type() const
{
    return LINEPROBER;
}

void LineProber::setPoint1(double* pot)
{
    if (ComDataGui3D::detail::ArrayComparator<double>()(d_ptr->pot1, pot))
        return;
    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->pot1, pot);
    update();
}

double* LineProber::point1() const
{
    return d_ptr->pot1;
}

void LineProber::setPoint2(double* pot)
{
    if (ComDataGui3D::detail::ArrayComparator<double>()(d_ptr->pot2, pot))
        return;

    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->pot2, pot);
    update();
}

double* LineProber::point2() const
{
    return d_ptr->pot2;
}

void LineProber::setPoints(double* pot1, double* pot2)
{
    if (ComDataGui3D::detail::ArrayComparator<double>()(d_ptr->pot1, pot1) &&
        ComDataGui3D::detail::ArrayComparator<double>()(d_ptr->pot2, pot2))
        return;

    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->pot1, pot1);
    ComDataGui3D::detail::ArrayAssigner<double>()(d_ptr->pot2, pot2);
    update();
}

void LineProber::update()
{
    //
    ComDataGui3D::vtkInitOnce(&d_ptr->firstBallActor);
    ComDataGui3D::vtkInitOnce(&d_ptr->secondBallActor);
    ComDataGui3D::vtkInitOnce(&d_ptr->lineActor);

    double bounds[6];
    double* pb = this->bounds();
    ComDataGui3D::detail::ArrayAssigner<double, 6>()(bounds, pb);
    double xRange = bounds[1] - bounds[0];
    double yRange = bounds[3] - bounds[2];
    double zRange = bounds[5] - bounds[4];
    double minRange = qMin(xRange, qMin(yRange, zRange));
    double radius = minRange / 50;

    CREATE_VTKOBJ(vtkSphereSource, ss1);
    ss1->SetCenter(d_ptr->pot1);
    ss1->SetRadius(radius);
    ss1->Update();

    CREATE_VTKOBJ(vtkSphereSource, ss2);
    ss2->SetCenter(d_ptr->pot2);
    ss2->SetRadius(radius);
    ss2->Update();

    CREATE_VTKOBJ(vtkPolyDataMapper, mapper1);
    mapper1->SetInputConnection(ss1->GetOutputPort());
    CREATE_VTKOBJ(vtkPolyDataMapper, mapper2);
    mapper2->SetInputConnection(ss2->GetOutputPort());

    d_ptr->firstBallActor->SetMapper(mapper1);
    d_ptr->secondBallActor->SetMapper(mapper2);

    CREATE_VTKOBJ(vtkLineSource, ls);
    ls->SetPoint1(d_ptr->pot1);
    ls->SetPoint2(d_ptr->pot2);
    ls->Update();

    CREATE_VTKOBJ(vtkPolyDataMapper, lineMapper);
    lineMapper->SetInputConnection(ls->GetOutputPort());
    d_ptr->lineActor->SetMapper(lineMapper);
    d_ptr->lineActor->GetProperty()->SetLineWidth(4);

    renderer()->AddActor(d_ptr->firstBallActor);
    renderer()->AddActor(d_ptr->secondBallActor);
    renderer()->AddActor(d_ptr->lineActor);

    Interactor::update();
}

void LineProber::attach(ComDataGui3D::PostProcessor *pp)
{
    if (!pp)
        return;
    Interactor::attach(pp);

    if (d_ptr->isPlotVisible)
        createPlot();

    double* bds = this->bounds();

    d_ptr->pot1[0] = bds[0];
    d_ptr->pot1[1] = bds[2] + (bds[3] - bds[2]) / 2;
    d_ptr->pot1[2] = bds[4] + (bds[5] - bds[4]) / 2;
    d_ptr->pot2[0] = bds[1];
    d_ptr->pot2[1] = bds[2] + (bds[3] - bds[2]) / 2;
    d_ptr->pot2[2] = bds[4] + (bds[5] - bds[4]) / 2;

    update();
    setSpecialInteractorStyle();
}

void LineProber::detach()
{
    renderer()->RemoveActor(d_ptr->firstBallActor);
    renderer()->RemoveActor(d_ptr->secondBallActor);
    renderer()->RemoveActor(d_ptr->lineActor);
    Interactor::detach();

    destroyPlot();
}

void LineProber::setData(vtkPointData *pd)
{
    if (!d_ptr->plot)
        return;

    d_ptr->plot->clearGraphs();
    int numOfArray = pd->GetNumberOfArrays();
    int colorCount = sizeof(DefaultColors) / sizeof(DefaultColors[0]);
    for (int i = 0; i < numOfArray; ++i) {
        QString arrayName(pd->GetArrayName(i));
        // skip this kind of data
        if (arrayName.compare("vtkValidPointMask", Qt::CaseInsensitive) == 0)
            continue;

        d_ptr->plot->addGraph();
        d_ptr->plot->graph(i)->setName(arrayName);
        d_ptr->plot->graph(i)->setPen(QColor(DefaultColors[i % colorCount]));
        vtkDataArray* da = pd->GetArray(i);
        int numOfTuples = da->GetNumberOfTuples();
        QVector<double> x(numOfTuples), y(numOfTuples);
        for (int dataIndex = 0; dataIndex < numOfTuples; ++dataIndex) {
            x.append(dataIndex);
            double value = da->GetTuple1(dataIndex);
            y.append(value);
        }
        d_ptr->plot->graph(i)->setData(x, y);
        d_ptr->plot->graph(i)->rescaleAxes(true);
    }
    d_ptr->plot->replot();
}

void LineProber::setVisible(bool visible)
{
    if (d_ptr->firstBallActor && d_ptr->firstBallActor->GetVisibility() != visible) {
        d_ptr->firstBallActor->SetVisibility(visible);
        d_ptr->secondBallActor->SetVisibility(visible);
        d_ptr->lineActor->SetVisibility(visible);

        if (d_ptr->plot)
            d_ptr->plot->setVisible(visible);

        Interactor::setVisible(visible);
    }
}

bool LineProber::isVisible() const
{
    if (d_ptr->firstBallActor)
        return d_ptr->firstBallActor->GetVisibility();
    return Interactor::isVisible();
}

void LineProber::setPlotVisible(bool visible)
{
    if (d_ptr->isPlotVisible != visible) {
        d_ptr->isPlotVisible = visible;
        if (visible)
            createPlot();
        else
            destroyPlot();
    }
}

bool LineProber::isPlotVisible() const
{
    return d_ptr->isPlotVisible;
}

void LineProber::onMouseReleased()
{
    if (processor())
        processor()->apply();
}

void LineProber::createPlot()
{
    if (!d_ptr->plot) {
        QSplitter* splitter = qobject_cast<QSplitter*>(graph()->parent());
        d_ptr->plot = new QCustomPlot(graph());
        splitter->addWidget(d_ptr->plot);

        d_ptr->plot->setAttribute(Qt::WA_TranslucentBackground);
        d_ptr->plot->xAxis2->setVisible(true);
        d_ptr->plot->xAxis2->setTickLabels(false);
        d_ptr->plot->yAxis2->setVisible(true);
        d_ptr->plot->yAxis2->setTickLabels(false);
        d_ptr->plot->legend->setVisible(true);
        d_ptr->plot->legend->setFont(QFont("Helvetica",9));
        d_ptr->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        connect(d_ptr->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), d_ptr->plot->xAxis2, SLOT(setRange(QCPRange)));
        connect(d_ptr->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), d_ptr->plot->yAxis2, SLOT(setRange(QCPRange)));
        d_ptr->plot->show();
        d_ptr->plot->clearGraphs();
    }
}

void LineProber::destroyPlot()
{
    if (d_ptr->plot) {
        delete d_ptr->plot;
        d_ptr->plot = nullptr;
    }
}

void LineProber::setSpecialInteractorStyle()
{
    MoveLineInteractorStyle* style = MoveLineInteractorStyle::New();
    style->setGraph(graph());
    style->SetDefaultRenderer(renderer());
    style->setBounds(bounds());
    style->setActors(d_ptr->firstBallActor, d_ptr->secondBallActor, d_ptr->lineActor);
    style->setPoints(d_ptr->pot1, d_ptr->pot2);

    connect(graph(), SIGNAL(clickPositionChanged(QPoint)), style, SLOT(setClickPosition(QPoint)));
    connect(graph(), SIGNAL(movePositionChanged(QPoint)), style, SLOT(setMovePosition(QPoint)));
    connect(style, SIGNAL(point1Changed(double*)), this, SIGNAL(point1Changed(double*)));
    connect(style, SIGNAL(point2Changed(double*)), this, SIGNAL(point2Changed(double*)));
    connect(style, SIGNAL(pointsChanged(double*,double*)), this, SIGNAL(pointsChanged(double*,double*)));

    connect(style, SIGNAL(point1Changed(double*)), this, SLOT(setPoint1(double*)));
    connect(style, SIGNAL(point2Changed(double*)), this, SLOT(setPoint2(double*)));
    connect(style, SIGNAL(pointsChanged(double*,double*)), this, SLOT(setPoints(double*,double*)));
    connect(style, SIGNAL(mouseReleased()), this, SLOT(onMouseReleased()));

    graph()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
}
