#include "probewindow.h"
#include "ui_generalfilterwindow.h"
#include "ui_probeconfig.h"
#include "qcustomplot.h"

#include <utils/signalblocker.h>
#include <utils/utils.h>
#include <utils/colorseries.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/linewidgetobserver.h>
#include <vtkutils/spherewidgetobserver.h>
#include <vtkutils/implicitplanewidgetobserver.h>

#include <vtkProbeFilter.h>
#include <vtkLineWidget.h>
#include <vtkSphereWidget.h>
#include <vtkBoxWidget.h>
#include <vtkPlaneWidget.h>
#include <vtkImplicitPlaneWidget.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>

#include <QDebug>

ProbeWindow::ProbeWindow(QWidget* parent) : FilterWindow(parent)
{
    createUi();

    m_configUi->sphereGroupBox->hide();
    m_configUi->implicitPlaneGroupBox->hide();

    m_plotWidget = new QCustomPlot(this);
    m_plotWidget->xAxis2->setVisible(true);
    m_plotWidget->xAxis2->setTickLabels(false);
    m_plotWidget->yAxis2->setVisible(true);
    m_plotWidget->yAxis2->setTickLabels(false);
    m_plotWidget->legend->setVisible(true);
    m_plotWidget->legend->setFont(QFont("Helvetica",9));
    m_plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    connect(m_plotWidget->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plotWidget->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_plotWidget->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plotWidget->yAxis2, SLOT(setRange(QCPRange)));
    m_plotWidget->clearGraphs();
    m_configUi->plotLayout->addWidget(m_plotWidget);

    setWindowTitle(tr("Probe"));
}

void ProbeWindow::apply()
{
    VTK_CREATE(vtkProbeFilter, probe);
    probe->SetInputData(m_dataObject);

    switch (m_widgetType) {
    case WT_Line:
    {
        VTK_CREATE(vtkLineSource, ls);
        ls->SetPoint1(m_linePoint1);
        ls->SetPoint2(m_linePoint2);
        ls->Update();
        probe->SetSourceData(ls->GetOutput());
    }
        break;

    case WT_Sphere:
    {
        VTK_CREATE(vtkSphereSource, ss);
        ss->SetCenter(m_sphereCenter);
        ss->Update();
        probe->SetSourceData(ss->GetOutput());
    }
        break;

    case WT_Box:
    {

    }
        break;

    case WT_ImplicitPlane:
    {
        VTK_CREATE(vtkPlaneSource, ps);
        ps->SetOrigin(m_planeOrigin);
        ps->SetNormal(m_planeNormal);
        ps->Update();
        probe->SetSourceData(ps->GetOutput());

    }
        break;

    }

    probe->Update();

    vtkPointData* pointData = probe->GetOutput()->GetPointData();

    Utils::ColorSeries colorSeries;
    colorSeries.setScheme(Utils::ColorSeries::Cool);

    m_plotWidget->clearGraphs();
    int numOfArray = pointData->GetNumberOfArrays();
    for (int i = 0; i < numOfArray; ++i) {
        QString arrayName(pointData->GetArrayName(i));
        // skip this kind of data
        if (arrayName.compare("vtkValidPointMask", Qt::CaseInsensitive) == 0)
            continue;

        m_plotWidget->addGraph();
        m_plotWidget->graph(i)->setName(arrayName);
        m_plotWidget->graph(i)->setPen(colorSeries.nextColor());
        vtkDataArray* da = pointData->GetArray(i);
        int numOfTuples = da->GetNumberOfTuples();
        QVector<double> x(numOfTuples), y(numOfTuples);
        for (int dataIndex = 0; dataIndex < numOfTuples; ++dataIndex) {
            x.append(dataIndex);
            double value = da->GetTuple1(dataIndex);
            y.append(value);
        }
        m_plotWidget->graph(i)->setData(x, y);
        m_plotWidget->graph(i)->rescaleAxes(true);
    }
    m_plotWidget->replot();
}

void ProbeWindow::on_openButton_clicked()
{
    browseFile();
    m_configUi->fileEdit->setText(fileName());
}

void ProbeWindow::on_sourceCombo_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        m_configUi->lineGroupBox->show();
        m_configUi->sphereGroupBox->hide();
        m_configUi->implicitPlaneGroupBox->hide();
        break;

    case 1:
        m_configUi->lineGroupBox->hide();
        m_configUi->sphereGroupBox->show();
        m_configUi->implicitPlaneGroupBox->hide();
        break;

    case 3:
        m_configUi->implicitPlaneGroupBox->show();
        m_configUi->lineGroupBox->hide();
        m_configUi->sphereGroupBox->hide();
        break;
    }

    m_widgetType = static_cast<WidgetType>(index);
    showProbeWidget();

    apply();
}

void ProbeWindow::onLineWidgetPointsChanged(double* point1, double* point2)
{
    if (Utils::ArrayComparator<double>()(m_linePoint1, point1) &&
        Utils::ArrayComparator<double>()(m_linePoint2, point2)) {
        qDebug() << "line point1 & ponit2 are not changed.";
        return;
    }

    Utils::SignalBlocker sb(m_configUi->linePoint1XSpinBox);
    sb.addObject(m_configUi->linePoint1YSpinBox);
    sb.addObject(m_configUi->linePoint1ZSpinBox);
    sb.addObject(m_configUi->linePoint2XSpinBox);
    sb.addObject(m_configUi->linePoint2YSpinBox);
    sb.addObject(m_configUi->linePoint2ZSpinBox);

    m_configUi->linePoint1XSpinBox->setValue(point1[0]);
    m_configUi->linePoint1YSpinBox->setValue(point1[1]);
    m_configUi->linePoint1ZSpinBox->setValue(point1[2]);
    m_configUi->linePoint2ZSpinBox->setValue(point2[0]);
    m_configUi->linePoint2ZSpinBox->setValue(point2[1]);
    m_configUi->linePoint2ZSpinBox->setValue(point2[2]);

    Utils::ArrayAssigner<double>()(m_linePoint1, point1);
    Utils::ArrayAssigner<double>()(m_linePoint2, point2);
    apply();
}

void ProbeWindow::onSphereWidgetCenterChanged(double* center)
{
    if (Utils::ArrayComparator<double>()(m_sphereCenter, center)) {
        qDebug() << "sphere center is not changed.";
        return;
    }

    Utils::SignalBlocker sb(m_configUi->sphereCenterXSpinBox);
    sb.addObject(m_configUi->sphereCenterYSpinBox);
    sb.addObject(m_configUi->sphereCenterZSpinBox);
    m_configUi->sphereCenterXSpinBox->setValue(center[0]);
    m_configUi->sphereCenterYSpinBox->setValue(center[1]);
    m_configUi->sphereCenterZSpinBox->setValue(center[2]);

    Utils::ArrayAssigner<double>()(m_sphereCenter, center);
    apply();
}

void ProbeWindow::onImplicitPlaneWidgetOriginChanged(double* origin)
{
    if (Utils::ArrayComparator<double>()(m_planeOrigin, origin)) {
        qDebug() << "plane origin is not changed.";
        return;
    }

    Utils::SignalBlocker sb(m_configUi->originXSpinBox);
    sb.addObject(m_configUi->originYSpinBox);
    sb.addObject(m_configUi->originZSpinBox);

    m_configUi->originXSpinBox->setValue(origin[0]);
    m_configUi->originYSpinBox->setValue(origin[1]);
    m_configUi->originZSpinBox->setValue(origin[2]);

    Utils::ArrayAssigner<double>()(m_planeOrigin, origin);
    apply();
}

void ProbeWindow::onImplicitPlaneWidgetNormalChanged(double* normal)
{
    if (Utils::ArrayComparator<double>()(m_planeNormal, normal)) {
        qDebug() << "plane normal is not changed.";
        return;
    }

    Utils::SignalBlocker sb(m_configUi->normalXSpinBox);
    sb.addObject(m_configUi->normalYSpinBox);
    sb.addObject(m_configUi->normalZSpinBox);

    m_configUi->normalXSpinBox->setValue(normal[0]);
    m_configUi->normalYSpinBox->setValue(normal[1]);
    m_configUi->normalZSpinBox->setValue(normal[2]);

    Utils::ArrayAssigner<double>()(m_planeNormal, normal);
    apply();
}

void ProbeWindow::modelReady()
{
    FilterWindow::modelReady();
    showProbeWidget();
}

void ProbeWindow::createUi()
{
    m_configUi = new Ui::ProbeConfig;
    setupConfigWidget(m_configUi);

    FilterWindow::createUi();
}

void ProbeWindow::showProbeWidget()
{
    switch (m_widgetType) {
    case WT_Line:
        if (!m_lineWidget) {
            m_lineWidget = vtkLineWidget::New();
            m_lineWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_lineWidget->SetPlaceFactor(1);
            m_lineWidget->SetClampToBounds(1);
            m_lineWidget->SetProp3D(m_modelActor);
            m_lineWidget->PlaceWidget();
//            m_lineWidget->AddObserver(vtkCommand::EndInteractionEvent, new LineCallback);
            VtkUtils::LineWidgetObserver* observer = new VtkUtils::LineWidgetObserver(this);
            connect(observer, SIGNAL(pointsChanged(double*,double*)), this, SLOT(onLineWidgetPointsChanged(double*,double*)));
            observer->attach(m_lineWidget);
        }

        m_lineWidget->On();

        // disable the other two
        if (m_sphereWidget)
            m_sphereWidget->Off();
        if (m_boxWidget)
            m_boxWidget->Off();
        if (m_implicitPlaneWidget)
            m_implicitPlaneWidget->Off();

        break;

    case WT_Sphere:
        if (!m_sphereWidget) {
            m_sphereWidget = vtkSphereWidget::New();
            m_sphereWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_sphereWidget->SetPlaceFactor(1);
            m_sphereWidget->SetProp3D(m_modelActor);
            m_sphereWidget->PlaceWidget();
            VtkUtils::SphereWidgetObserver* observer = new VtkUtils::SphereWidgetObserver(this);
            connect(observer, SIGNAL(centerChanged(double*)), this, SLOT(onSphereWidgetCenterChanged(double*)));
            observer->attach(m_sphereWidget);
            m_sphereRadius = m_sphereWidget->GetRadius();
        }
        m_sphereWidget->On();
        m_sphereWidget->SetRadius(m_sphereRadius);

        // disable the other two
        if (m_lineWidget)
            m_lineWidget->Off();
        if (m_boxWidget)
            m_boxWidget->Off();
        if (m_implicitPlaneWidget)
            m_implicitPlaneWidget->Off();

        break;

    case WT_Box:
        if (!m_boxWidget) {
            m_boxWidget = vtkBoxWidget::New();
            m_boxWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_boxWidget->SetPlaceFactor(1);
            m_boxWidget->SetProp3D(m_modelActor);
            m_boxWidget->PlaceWidget();
        }
        m_boxWidget->On();

        // disable the other two
        if (m_lineWidget)
            m_lineWidget->Off();
        if (m_sphereWidget)
            m_sphereWidget->Off();
        if (m_implicitPlaneWidget)
            m_implicitPlaneWidget->Off();

        break;

    case WT_ImplicitPlane:
        if (!m_implicitPlaneWidget) {
            m_implicitPlaneWidget = vtkImplicitPlaneWidget::New();
            m_implicitPlaneWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_implicitPlaneWidget->SetPlaceFactor(1);
            m_implicitPlaneWidget->SetProp3D(m_modelActor);
            m_implicitPlaneWidget->SetOutlineTranslation(0); // make the outline non-movable
            m_implicitPlaneWidget->GetPlaneProperty()->SetOpacity(0.3);
            m_implicitPlaneWidget->GetPlaneProperty()->SetColor(.0, .0, .0);

            double bounds[6];
            if (isValidPolyData()) {
                vtkPolyData* polyData = vtkPolyData::SafeDownCast(m_dataObject);
                polyData->GetBounds(bounds);
                m_implicitPlaneWidget->SetOrigin(polyData->GetCenter());
            } else if (isValidDataSet()) {
                vtkDataSet* dataSet = vtkDataSet::SafeDownCast(m_dataObject);
                dataSet->GetBounds(bounds);
                m_implicitPlaneWidget->SetOrigin(dataSet->GetCenter());
            }
//            m_implicitPlaneWidget->SetOutsideBounds(0);
            m_implicitPlaneWidget->PlaceWidget(bounds);

            VtkUtils::ImplicitPlaneWidgetObserver* observer = new VtkUtils::ImplicitPlaneWidgetObserver(this);
            connect(observer, SIGNAL(originChanged(double*)), this, SLOT(onImplicitPlaneWidgetOriginChanged(double*)));
            connect(observer, SIGNAL(normalChanged(double*)), this, SLOT(onImplicitPlaneWidgetNormalChanged(double*)));
            observer->attach(m_implicitPlaneWidget);
        }
        m_implicitPlaneWidget->On();
        m_implicitPlaneWidget->UpdatePlacement();

        // disable the other two
        if (m_lineWidget)
            m_lineWidget->Off();
        if (m_sphereWidget)
            m_sphereWidget->Off();
        if (m_boxWidget)
            m_boxWidget->Off();

        break;
    }


    applyDisplayEffect();
}

void ProbeWindow::on_sphereRadius_valueChanged(double arg1)
{
    m_sphereRadius = arg1;
    showProbeWidget();
    apply();
}
