#include "streamlinewindow.h"
#include "ui_streamlineconfig.h"
#include "ui_generalfilterwindow.h"

#include <utils/signalblocker.h>
#include <utils/utils.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/pointwidgetobserver.h>
#include <vtkutils/linewidgetobserver.h>

#include <vtkRuledSurfaceFilter.h>
#include <vtkPointSource.h>
#include <vtkLineSource.h>
#include <vtkRungeKutta2.h>
#include <vtkRungeKutta4.h>
#include <vtkRungeKutta45.h>
#include <vtkLODActor.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkDataSet.h>
#include <vtkProperty.h>
#include <vtkTubeFilter.h>
#include <vtkLineWidget.h>
#include <vtkPointWidget.h>

StreamlineWindow::StreamlineWindow(QWidget* parent) : FilterWindow(parent)
{
    createUi();
    setWindowTitle(tr("Streamline"));

    m_configUi->configTubeGroupBox->hide();
    m_configUi->configRuledSurfaceGroupBox->hide();
    showConfigGroupBox(0);
}

StreamlineWindow::~StreamlineWindow()
{
    Utils::vtkSafeDelete(m_lineWidget);
    Utils::vtkSafeDelete(m_pointWidget);
}

void StreamlineWindow::apply()
{
    if (!m_dataObject)
        return;

    VTK_CREATE(vtkStreamTracer, streamline);
    streamline->SetInputData(m_dataObject);
    streamline->SetIntegrationDirection(m_integrationDirection);

    if (m_source == Point) {
        VTK_CREATE(vtkPointSource, ps);
        ps->SetCenter(m_center);
        ps->SetNumberOfPoints(m_numberOfPoints);
        ps->SetRadius(m_radius);
        ps->Update();
        streamline->SetSourceConnection(ps->GetOutputPort());
    } else {
        VTK_CREATE(vtkLineSource, ls);
        ls->SetResolution(30);
        ls->SetPoint1(m_point1);
        ls->SetPoint2(m_point2);
        ls->Update();
        streamline->SetSourceConnection(ls->GetOutputPort());
    }

    streamline->SetMaximumPropagation(100);
    streamline->SetInitialIntegrationStep(0.1);

    switch (m_integratorType) {
    case 0:
    {
        VTK_CREATE(vtkRungeKutta2, integ);
        streamline->SetIntegrator(integ);
        break;
    }

    case 1:
    {
        VTK_CREATE(vtkRungeKutta4, integ);
        streamline->SetIntegrator(integ);
        break;
    }

    case 2:
    {
        VTK_CREATE(vtkRungeKutta45, integ);
        streamline->SetIntegrator(integ);
        break;
    }

    } // switch

    streamline->Update();

    VTK_CREATE(vtkPolyDataMapper, streamLineMapper);

    switch (m_displayMode) {
    case None:
        streamLineMapper->SetInputConnection(streamline->GetOutputPort());
        break;

    case Surface:
    {
        VTK_CREATE(vtkRuledSurfaceFilter, surfaceFilter);
        surfaceFilter->SetOffset(0);
        surfaceFilter->SetOnRatio(.2);
        surfaceFilter->PassLinesOn();
        surfaceFilter->SetRuledModeToPointWalk();
        surfaceFilter->SetDistanceFactor(30);
#if 0
        surfaceFilter->SetOffset(m_ruledSurfaceParams.offset);
        surfaceFilter->SetOnRatio(m_ruledSurfaceParams.onRatio);
        surfaceFilter->SetPassLines(m_ruledSurfaceParams.passLines);
//        surfaceFilter->SetRuledMode(m_ruledSurfaceParams.ruledMode);
        surfaceFilter->SetDistanceFactor(m_ruledSurfaceParams.distanceFactor);
        surfaceFilter->SetResolution(m_ruledSurfaceParams.resolution);
        surfaceFilter->SetOrientLoops(m_ruledSurfaceParams.orientLoops);
        surfaceFilter->SetCloseSurface(m_ruledSurfaceParams.closeSurface);
        surfaceFilter->SetInputConnection(streamline->GetOutputPort());
#endif
        streamLineMapper->SetInputConnection(surfaceFilter->GetOutputPort());
    }
        break;

    case Tube:
    {
        VTK_CREATE(vtkTubeFilter, tubeFilter);
        tubeFilter->SetNumberOfSides(m_tubeParams.numberOfSides);
        tubeFilter->SetRadiusFactor(m_tubeParams.radiusFactor);
        tubeFilter->SetCapping(m_tubeParams.capping);
        tubeFilter->SetInputConnection(streamline->GetOutputPort());
        streamLineMapper->SetInputConnection(tubeFilter->GetOutputPort());
    }
        break;
    }

    VtkUtils::vtkInitOnce(&m_filterActor);
    m_filterActor->SetMapper(streamLineMapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_filterActor);

    applyDisplayEffect();
}

void StreamlineWindow::modelReady()
{
    FilterWindow::modelReady();

    if (!m_dataObject)
        return;

    vtkPolyData* polydata = vtkPolyData::SafeDownCast(m_dataObject);
    vtkDataSet* dataset = vtkDataSet::SafeDownCast(m_dataObject);
    double bounds[6];
    double center[3];

    if (polydata) {
        polydata->GetBounds(bounds);
        polydata->GetCenter(center);
    } else if (dataset) {
        dataset->GetBounds(bounds);
        dataset->GetCenter(center);
    }

    Utils::ArrayAssigner<double>()(m_center, center);

    m_point1[0] = bounds[0];
    m_point1[1] = center[1];
    m_point1[2] = center[2];
    m_point2[0] = bounds[1];
    m_point2[1] = center[1];
    m_point2[2] = center[2];

    m_configUi->centerXSpinBox->setRange(bounds[0], bounds[1]);
    m_configUi->centerYSpinBox->setRange(bounds[2], bounds[3]);
    m_configUi->centerZSpinBox->setRange(bounds[4], bounds[5]);
    m_configUi->point1XSpinBox->setRange(bounds[0], bounds[1]);
    m_configUi->point1YSpinBox->setRange(bounds[2], bounds[3]);
    m_configUi->point1ZSpinBox->setRange(bounds[4], bounds[5]);
    m_configUi->point2XSpinBox->setRange(bounds[0], bounds[1]);
    m_configUi->point2YSpinBox->setRange(bounds[2], bounds[3]);
    m_configUi->point2ZSpinBox->setRange(bounds[4], bounds[5]);

    Utils::SignalBlocker sb(m_configUi->centerXSpinBox);
    sb.addObject(m_configUi->centerYSpinBox);
    sb.addObject(m_configUi->centerZSpinBox);
    sb.addObject(m_configUi->point1XSpinBox);
    sb.addObject(m_configUi->point1YSpinBox);
    sb.addObject(m_configUi->point1ZSpinBox);
    sb.addObject(m_configUi->point2XSpinBox);
    sb.addObject(m_configUi->point2YSpinBox);
    sb.addObject(m_configUi->point2ZSpinBox);

    m_configUi->centerXSpinBox->setValue(m_center[0]);
    m_configUi->centerYSpinBox->setValue(m_center[1]);
    m_configUi->centerZSpinBox->setValue(m_center[2]);
    m_configUi->point1XSpinBox->setValue(m_point1[0]);
    m_configUi->point1YSpinBox->setValue(m_point1[1]);
    m_configUi->point1ZSpinBox->setValue(m_point1[2]);
    m_configUi->point2XSpinBox->setValue(m_point2[0]);
    m_configUi->point2YSpinBox->setValue(m_point2[1]);
    m_configUi->point2ZSpinBox->setValue(m_point2[2]);

    showConfigGroupBox(m_configUi->sourceCombo->currentIndex());

    if (m_source == Point)
        updatePointActor();
    else if (m_source == Line)
        updateLineActor();
}

void StreamlineWindow::createUi()
{
    m_configUi = new Ui::StreamlineConfig;
    setupConfigWidget(m_configUi);

    FilterWindow::createUi();
}

void StreamlineWindow::showConfigGroupBox(int index)
{
    if (index == 0) {
        m_configUi->configPointGroupBox->show();
        m_configUi->configLineGroupBox->hide();

        if (m_lineWidget)
            m_lineWidget->Off();
    } else if (index == 1) {
        m_configUi->configLineGroupBox->show();
        m_configUi->configPointGroupBox->hide();

        if (m_pointWidget)
            m_pointWidget->Off();
    }

    update();
}

void StreamlineWindow::updatePointActor()
{
    if (!m_pointWidget) {
        m_pointWidget = vtkPointWidget::New();
        VtkUtils::PointWidgetObserver* observer = new VtkUtils::PointWidgetObserver(this);
        connect(observer, SIGNAL(positionChanged(double*)), this, SLOT(onPointPositionChanged(double*)));
        observer->attach(m_pointWidget);
        m_pointWidget->SetProp3D(m_modelActor);
        m_pointWidget->SetInteractor(m_vtkWidget->GetInteractor());
        m_pointWidget->PlaceWidget();
        m_pointWidget->SetPlaceFactor(1.0);
        m_pointWidget->AllOff();
        m_pointWidget->GetProperty()->SetPointSize(4);
    }
    m_pointWidget->SetPosition(m_center);
    m_pointWidget->On();
    update();
}

void StreamlineWindow::updateLineActor()
{
    if (!m_lineWidget) {
        m_lineWidget = vtkLineWidget::New();
        VtkUtils::LineWidgetObserver* observer = new VtkUtils::LineWidgetObserver(this);
        connect(observer, SIGNAL(pointsChanged(double*,double*)), this, SLOT(onLinePointsChanged(double*,double*)));
        observer->attach(m_lineWidget);
        m_lineWidget->SetProp3D(m_modelActor);
        m_lineWidget->SetInteractor(m_vtkWidget->GetInteractor());
        m_lineWidget->SetPlaceFactor(1.0);
        m_lineWidget->SetClampToBounds(0);
        m_lineWidget->PlaceWidget();
    }
    m_lineWidget->SetPoint1(m_point1);
    m_lineWidget->SetPoint1(m_point2);
    m_lineWidget->On();
    update();
}

void StreamlineWindow::on_sourceCombo_currentIndexChanged(int index)
{
    showConfigGroupBox(index);

    m_source = static_cast<SourceType>(index);
    if (m_source == Point)
        updatePointActor();
    else if (m_source)
        updateLineActor();

    apply();
}

void StreamlineWindow::on_openButton_clicked()
{
    browseFile();
    m_configUi->fileEdit->setText(m_fileName);
}

void StreamlineWindow::on_numOfPointsSpinBox_valueChanged(int arg1)
{
    m_numberOfPoints = arg1;
    apply();
}

void StreamlineWindow::on_radiusSpinBox_valueChanged(double arg1)
{
    m_radius = arg1;
    apply();
}

void StreamlineWindow::on_centerXSpinBox_valueChanged(double arg1)
{
    m_center[0] = arg1;
    updatePointActor();
    apply();
}

void StreamlineWindow::on_centerYSpinBox_valueChanged(double arg1)
{
   m_center[1] = arg1;
   updatePointActor();
   apply();
}

void StreamlineWindow::on_centerZSpinBox_valueChanged(double arg1)
{
    m_center[2] = arg1;
    updatePointActor();
    apply();
}

void StreamlineWindow::on_integratorTypeCombo_currentIndexChanged(int index)
{
    m_integratorType = index;
    apply();
}

void StreamlineWindow::on_integrationDirectionCombo_currentIndexChanged(int index)
{
    m_integrationDirection = index;
    apply();
}

void StreamlineWindow::on_point1XSpinBox_valueChanged(double arg1)
{
    m_point1[0] = arg1;
    updateLineActor();
    apply();
}

void StreamlineWindow::on_point1YSpinBox_valueChanged(double arg1)
{
    m_point1[1] = arg1;
    updateLineActor();
    apply();
}

void StreamlineWindow::on_point1ZSpinBox_valueChanged(double arg1)
{
    m_point1[2] = arg1;
    updateLineActor();
    apply();
}

void StreamlineWindow::on_point2XSpinBox_valueChanged(double arg1)
{
    m_point2[0] = arg1;
    updateLineActor();
    apply();
}

void StreamlineWindow::on_point2YSpinBox_valueChanged(double arg1)
{
    m_point2[1] = arg1;
    updateLineActor();
    apply();
}

void StreamlineWindow::on_point2ZSpinBox_valueChanged(double arg1)
{
    m_point2[2] = arg1;
    updateLineActor();
    apply();
}

void StreamlineWindow::on_displayModeCombo_currentIndexChanged(int index)
{
    m_displayMode = static_cast<DisplayMode>(index);

    switch (index) {
    case None:
        m_configUi->configTubeGroupBox->hide();
        m_configUi->configRuledSurfaceGroupBox->hide();
        break;

    case Tube:
        m_configUi->configTubeGroupBox->show();
        m_configUi->configRuledSurfaceGroupBox->hide();
        break;

    case Surface:
        m_configUi->configTubeGroupBox->hide();
        m_configUi->configRuledSurfaceGroupBox->show();
        break;
    }

    apply();
}

void StreamlineWindow::on_displayEffectCombo_currentIndexChanged(int index)
{
    m_displayEffect = static_cast<DisplayEffect>(index);
    applyDisplayEffect();
}

void StreamlineWindow::on_numOfSidesSpinBox_valueChanged(int arg1)
{
    m_tubeParams.numberOfSides = arg1;
    apply();
}

void StreamlineWindow::on_radiusFactorSpinBox_valueChanged(double arg1)
{
    m_tubeParams.radiusFactor = arg1;
    apply();
}

void StreamlineWindow::on_cappingCheckBox_toggled(bool checked)
{
    m_tubeParams.capping = checked;
    apply();
}

void StreamlineWindow::on_distanceFactorSpinBox_valueChanged(int arg1)
{
    m_ruledSurfaceParams.distanceFactor = arg1;
    apply();
}

void StreamlineWindow::on_onRatioSpinBox_valueChanged(int arg1)
{
    m_ruledSurfaceParams.onRatio = arg1;
    apply();
}

void StreamlineWindow::on_offsetSpinBox_valueChanged(int arg1)
{
    m_ruledSurfaceParams.offset = arg1;
    apply();
}

void StreamlineWindow::on_passLinesCheckBox_toggled(bool checked)
{
    m_ruledSurfaceParams.passLines = checked;
    apply();
}

void StreamlineWindow::on_closeSurfaceCheckBox_toggled(bool checked)
{
    m_ruledSurfaceParams.closeSurface = checked;
    apply();
}

void StreamlineWindow::on_orientLoopsCheckBox_toggled(bool checked)
{
    m_ruledSurfaceParams.orientLoops = checked;
    apply();
}

void StreamlineWindow::on_ruledModeCombo_currentIndexChanged(int index)
{
    m_ruledSurfaceParams.ruledMode = index;
    apply();
}

void StreamlineWindow::on_resolutionSpinBox_valueChanged(int arg1)
{
//    m_ruledSurfaceParams.resolution = arg1;
//    apply();
}

void StreamlineWindow::onLinePointsChanged(double* point1, double* point2)
{
    if (Utils::ArrayComparator<double>()(m_point1, point1) &&
        Utils::ArrayComparator<double>()(m_point2, point2))
        return;

    Utils::ArrayAssigner<double>()(m_point1, point1);
    Utils::ArrayAssigner<double>()(m_point2, point2);
    apply();
}

void StreamlineWindow::onPointPositionChanged(double* position)
{
    if (Utils::ArrayComparator<double>()(m_center, position))
        return;

    Utils::ArrayAssigner<double>()(m_center, position);
    apply();
}

