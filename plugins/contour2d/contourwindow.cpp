#include "contourwindow.h"
#include "ui_contourwindow.h"
#include "contour2dinteractorstyle.h"
#include "abstractcontourgenerator.h"
#include "smoothcontourgenerator.h"
#include "bandedcontourgenerator.h"

#include <utils/utils.h>
#include <utils/pointsreader.h>
#include <utils/tablemodel.h>
#include <utils/point3f.h>
#include <utils/pointstomodelconverter.h>
#include <utils/modeltopointsconverter.h>
#include <utils/signalblocker.h>

#include <widgets/gradientcombobox.h>

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>
#include <vtkutils/pointstopolydataconverter.h>
#include <vtkutils/linewidgetobserver.h>

#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include <vtkProperty.h>
#include <vtkLineWidget.h>
#include <vtkPlaneSource.h>
#include <vtkDelaunay2D.h>
#include <vtkContourFilter.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkMath.h>
#include <vtkLabeledDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkCutter.h>
#include <vtkFieldData.h>
#include <vtkPlane.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkAppendPolyData.h>
#include <vtkTextActor.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkAxis.h>
#include <vtkColorSeries.h>

#include <QThreadPool>
#include <QFileDialog>
#include <QDebug>

ContourWindow::ContourWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ContourWindow)
{
    init();
}

ContourWindow::~ContourWindow()
{
    delete m_ui;
}

void ContourWindow::createContour()
{
    if (m_isBandedContour) {
//        createBandedContour();
        fireupBandedContourGenerator();
    } else {
//        createSmoothContour();
        fireupSmoothContourGenerator();
    }
}

void ContourWindow::on_numOfContoursSpinBox_valueChanged(int arg1)
{
    if (m_numberOfContours != arg1) {
        m_numberOfContours = arg1;
        createContour();
    }
}

void ContourWindow::on_demoSourcesCombo_currentIndexChanged(int index)
{
    if (index == 0)
        return;

    QString datafile = m_ui->demoSourcesCombo->itemText(index);

    // read demo
    Utils::PointsReader* reader = new Utils::PointsReader(QString(":%1").arg(datafile));
    reader->setAutoDelete(false);
    QThreadPool::globalInstance()->start(reader);
    connect(reader, SIGNAL(finished()), this, SLOT(onPointReaderFinished()));

    // prevent quick switching
    m_ui->demoSourcesCombo->setEnabled(false);
}

void ContourWindow::on_labelGroupBox_toggled(bool arg1)
{
    m_isLabelVisible = arg1;

    if (!arg1 && m_labelActor) {
        m_labelActor->SetVisibility(false);
        m_vtkWidget->update();
        return;
    }

    if (m_isBandedContour)
        fireupBandedContourGenerator();
    else
        fireupSmoothContourGenerator();
}

void ContourWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    m_tableModel->resize(m_tableModel->columnCount(), arg1);
    emit m_tableModel->layoutChanged();
    fireupModelToPointsConverter();
}

void ContourWindow::on_plotScalarsCheckBox_toggled(bool checked)
{
    m_ui->scalarPlotGroupBox->setVisible(checked);

    if (checked) {
        if (!m_lineWidget) {
            m_lineWidget = vtkLineWidget::New();
            m_lineWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_lineWidget->SetPlaceFactor(1);
            m_lineWidget->SetClampToBounds(1);
            m_lineWidget->SetProp3D(m_contourActor);

            VtkUtils::LineWidgetObserver* observer = new VtkUtils::LineWidgetObserver(this);
            observer->attach(m_lineWidget);
            connect(observer, SIGNAL(pointsChanged(double*,double*)), this, SLOT(onLineWidgetPointsChanged(double*,double*)));
        }

        VtkUtils::vtkInitOnce(m_linePointsActor);
        m_linePointsActor->SetVisibility(true);
        m_vtkWidget->defaultRenderer()->AddActor2D(m_linePointsActor);

        m_lineWidget->PlaceWidget();
        m_lineWidget->On();
    } else {
        if (m_lineWidget)
            m_lineWidget->Off();

        if (m_linePointsActor)
            m_linePointsActor->SetVisibility(false);
    }

    m_vtkWidget->update();
}

void ContourWindow::onPointReaderFinished()
{
    Utils::PointsReader* reader = qobject_cast<Utils::PointsReader*>(sender());
    QList<Utils::Point3F> points = reader->points();

    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(points.size());

    Utils::PointsToModelConverter* converter = new Utils::PointsToModelConverter(points, m_tableModel);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onPointsToModelConverterFinished()));
    QThreadPool::globalInstance()->start(converter);

    reader->deleteLater();
}

void ContourWindow::onPointsToPolyDataConverterFinished()
{
    VtkUtils::PointsToPolyDataConverter* converter = qobject_cast<VtkUtils::PointsToPolyDataConverter*>(sender());

    if (!m_polyData)
        m_polyData = vtkPolyData::New();
    m_polyData->DeepCopy(converter->polyData());

    createContour();
    converter->deleteLater();
}

void ContourWindow::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToPointsConverter();
}

void ContourWindow::onMousePressed(double* pos)
{
    if (!m_isPickPoint)
        return;
}

template <typename T>
static inline void normal(T* inPot1, T* inPot2, T* outPot)
{
    outPot[0] = -(inPot2[1] - inPot1[1]);
    outPot[1] = inPot2[0] - inPot1[0];
    outPot[2] = inPot1[2];
}

inline static bool pairLess(const QPair<double, double>& lhs, const QPair<double, double>& rhs)
{
    return lhs.first < rhs.first;
}

void ContourWindow::onLineWidgetPointsChanged(double* pot1, double* pot2)
{
    // update line points
    QString posInfo;
    posInfo = QString("Point1[%1,%2,%3], Point2[%4,%5,%6]").arg(pot1[0]).arg(pot1[1]).arg(pot1[2])
                                                           .arg(pot2[0]).arg(pot2[1]).arg(pot2[2]);
    m_linePointsActor->SetInput(posInfo.toUtf8().data());

    // calculate the two attached points
    double pot[3] = { (pot2[0] - pot1[0]) / 2, (pot2[1] - pot1[1]) / 2, pot1[2]};
    double normal[3];
    ::normal(pot1, pot2, normal);

    VTK_CREATE(vtkPlane, plane);
    plane->SetOrigin(pot);
    plane->SetNormal(normal);

    VTK_CREATE(vtkCutter, cutter);
    cutter->SetInputData(m_contourActor->GetMapper()->GetInput());
    cutter->SetCutFunction(plane);
    cutter->Update();

    vtkPolyData* output = cutter->GetOutput();
    vtkPointData* pointData = output->GetPointData();
    vtkDataArray* scalars = pointData->GetScalars();

    int numOfTuples = scalars->GetNumberOfTuples();
    if (numOfTuples == 0) {
        qDebug() << "no data available.";
        return;
    }

    if (!m_contextView) {
        m_plotWidget = new VtkUtils::VtkWidget(this);
        m_contextView = vtkContextView::New();
        m_contextView->SetRenderWindow(m_plotWidget->GetRenderWindow());
        m_contextView->GetRenderWindow()->SetMultiSamples(0);
        m_contextView->GetRenderWindow()->Render();

        VtkUtils::vtkInitOnce(m_chart);
        m_contextView->GetScene()->AddItem(m_chart);

        m_linePlot = m_chart->AddPlot(vtkChart::LINE);

        vtkSmartPointer<vtkColorSeries> colorSeries = vtkColorSeries::New();
        colorSeries->SetColorScheme(vtkColorSeries::COOL);

        auto clr = colorSeries->GetColor(0);
        m_linePlot->SetColor(clr.GetRed(), clr.GetGreen(), clr.GetBlue());

        m_ui->plotLayout->addWidget(m_plotWidget);
    }

    vtkSmartPointer<vtkTable> table = vtkTable::New();
    vtkSmartPointer<vtkDoubleArray> indexes = vtkDoubleArray::New();
    indexes->SetName("index");
    table->AddColumn(indexes);
    vtkSmartPointer<vtkDoubleArray> values = vtkDoubleArray::New();
    values->SetName("value");
    table->AddColumn(values);
    table->SetNumberOfRows(numOfTuples);

    double scalar;
    for (int i = 0; i < numOfTuples; ++i) {
        scalar = scalars->GetTuple1(i);
        table->SetValue(i, 0, i);
        table->SetValue(i, 1, scalar);
    }

    m_linePlot->SetInputData(table, 0, 1);
    m_plotWidget->update();
}

void ContourWindow::onContourGeneratorFinished()
{
    AbstractContourGenerator* generator = qobject_cast<AbstractContourGenerator*>(sender());
    vtkPolyData* contour = generator->contourData();

    double bounds[6];
    m_polyData->GetBounds(bounds);
    double zMin = bounds[4];
    double zMax = bounds[5];
    setScalars(zMin, zMax);

    double hsv1[3];
    double hsv2[3];
    Utils::qColor2HSV(m_lutColor1, hsv1);
    Utils::qColor2HSV(m_lutColor2, hsv2);

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(hsv1[0], hsv2[0]);
    lut->SetSaturationRange(hsv1[1], hsv2[1]);
    lut->SetValueRange(hsv1[2], hsv2[2]);
    lut->SetTableRange(scalarMin(), scalarMax());
    lut->SetNumberOfColors(m_numberOfContours);
    lut->Build();

    VtkUtils::vtkInitOnce(m_scalarBar);
    m_scalarBar->SetTitle("Scalar");
    m_scalarBar->SetLookupTable(lut);
    m_scalarBar->SetNumberOfLabels(m_numberOfContours);
    m_vtkWidget->defaultRenderer()->AddActor(m_scalarBar);

    VtkUtils::vtkInitOnce(m_cubeAxes);
    m_cubeAxes->SetInputData(contour);
    m_cubeAxes->SetZAxisVisibility(0);
    m_cubeAxes->SetFontFactor(1.0);
    m_cubeAxes->SetFlyModeToNone();
    m_cubeAxes->SetShowActualBounds(1);
    m_cubeAxes->SetNumberOfLabels(m_numberOfLabels);
    m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());

    m_vtkWidget->defaultRenderer()->AddViewProp(m_cubeAxes);

    VTK_CREATE(vtkPolyDataMapper, planeSurfaceMapper);
    planeSurfaceMapper->SetInputData(contour);
    planeSurfaceMapper->SetScalarRange(zMin, zMax);
    planeSurfaceMapper->SetLookupTable(lut);

    if (generator->type() == AbstractContourGenerator::Banded)
        planeSurfaceMapper->SetScalarModeToUseCellData();

    VtkUtils::vtkInitOnce(m_contourActor);
    m_contourActor->SetMapper(planeSurfaceMapper);

    if (generator->type() == AbstractContourGenerator::Banded)
        m_contourActor->GetProperty()->SetInterpolationToFlat();
    else if (generator->type() == AbstractContourGenerator::Smooth)
        m_contourActor->GetProperty()->SetInterpolationToPhong();

    m_vtkWidget->defaultRenderer()->AddActor(m_contourActor);

    if (generator->generateLines()) {
        VtkUtils::vtkInitOnce(m_contourLineActor);

        VTK_CREATE(vtkPolyDataMapper, contourLinesMapper);
        contourLinesMapper->SetInputData(generator->contourLinesData());
        contourLinesMapper->SetScalarRange(bounds[4], bounds[5]);
        contourLinesMapper->SetScalarModeToUseCellData();
        contourLinesMapper->ScalarVisibilityOff();
        m_contourLineActor->SetMapper(contourLinesMapper);
        m_contourLineActor->SetVisibility(m_isContourLineVisible);
        m_contourLineActor->GetProperty()->SetColor(.0, .0, .0);

        m_vtkWidget->defaultRenderer()->AddActor(m_contourLineActor);
    }

    if (generator->generateLabels()) {
        VtkUtils::vtkInitOnce(m_labeledDataMapper);
        m_labeledDataMapper->SetFieldDataName("Isovalues");
        m_labeledDataMapper->SetInputData(generator->contourLabelsData());
        m_labeledDataMapper->SetLabelModeToLabelScalars();
        m_labeledDataMapper->SetLabelFormat(m_labelFormat.toUtf8().data());
        m_labeledDataMapper->Update();

        VtkUtils::vtkInitOnce(m_labelActor);
        m_labelActor->SetMapper(m_labeledDataMapper);
        m_labelActor->SetVisibility(m_isLabelVisible);
        m_vtkWidget->defaultRenderer()->AddActor2D(m_labelActor);
    }

    m_vtkWidget->defaultRenderer()->ResetCamera();
    m_vtkWidget->update();

    generator->deleteLater();
    setTriggerWidgetsEnabled(true);
}

void ContourWindow::onGradientComboBoxActivated(int index)
{
    Q_UNUSED(index);
    Widgets::GradientComboBox* gcb = qobject_cast<Widgets::GradientComboBox*>(sender());

    m_lutColor1 = gcb->currentColor1();
    m_lutColor2 = gcb->currentColor2();

    double hsv1[3];
    double hsv2[3];
    Utils::qColor2HSV(m_lutColor1, hsv1);
    Utils::qColor2HSV(m_lutColor2, hsv2);

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(hsv1[0], hsv2[0]);
    lut->SetSaturationRange(hsv1[1], hsv2[1]);
    lut->SetValueRange(hsv1[2], hsv2[2]);
    lut->SetTableRange(scalarMin(), scalarMax());
    lut->SetNumberOfColors(m_numberOfContours);
    lut->Build();

    m_scalarBar->SetLookupTable(lut);
    m_contourActor->GetMapper()->SetLookupTable(lut);
    m_vtkWidget->update();
}

void ContourWindow::onPointsToModelConverterFinished()
{
    Utils::PointsToModelConverter* converter = qobject_cast<Utils::PointsToModelConverter*>(sender());
    emit m_tableModel->layoutChanged();
    converter->deleteLater();

    // only after finished converting, the new process can be started
    m_ui->demoSourcesCombo->setEnabled(true);
    fireupModelToPointsConverter();
}

void ContourWindow::init()
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Contour"));

    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    sb.addObject(m_ui->labelColorButton);
    sb.addObject(m_ui->numOfContoursSpinBox);
    sb.addObject(m_ui->showContourLineCheckBox);
    sb.addObject(m_ui->numOfLabelsSpinBox);
    sb.addObject(m_ui->labelFormatEdit);
    sb.addObject(m_ui->gradientCombo);

    m_ui->rowsSpinBox->setValue(DefaultRows);
    m_ui->labelColorButton->setCurrentColor(m_labelColor);
    m_ui->numOfContoursSpinBox->setValue(m_numberOfContours);
    m_ui->showContourLineCheckBox->setChecked(m_isContourLineVisible);
    m_ui->numOfLabelsSpinBox->setValue(m_numberOfLabels);
    m_ui->labelFormatEdit->setText(m_labelFormat);
    m_ui->scalarPlotGroupBox->hide();
    m_ui->gradientCombo->setCurrentIndex(3);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    Contour2DInteractorStyle* style = new Contour2DInteractorStyle(this);
    connect(style, SIGNAL(mousePressed(double*)), this, SLOT(onMousePressed(double*)));
    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

    m_tableModel = new Utils::TableModel(3, DefaultRows, this);
    m_tableModel->setHorizontalHeaderData(QVariantList() << "X" << "Y" << "Scalar");
    m_ui->dataTable->setModel(m_tableModel);
    m_vtkWidget->update();
    connect(m_ui->labelColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_tableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    connect(m_ui->gradientCombo, SIGNAL(activated(int)), this, SLOT(onGradientComboBoxActivated(int)));

    m_tableModel->random();
    fireupModelToPointsConverter();
}

void ContourWindow::fireupPointsReader(const QString& file)
{
    Utils::PointsReader* pointsReader = new Utils::PointsReader(file);
    pointsReader->setAutoDelete(false);
    connect(pointsReader, SIGNAL(finished()), this, SLOT(onPointReaderFinished()));
    QThreadPool::globalInstance()->start(pointsReader);
}

void ContourWindow::fireupModelToPointsConverter()
{
    Utils::ModelToPointsConverter* mtpConverter = new Utils::ModelToPointsConverter(m_tableModel);
    mtpConverter->setAutoDelete(false);
    connect(mtpConverter, SIGNAL(finished()), this, SLOT(onModelToPointsConverterFinished()));
    QThreadPool::globalInstance()->start(mtpConverter);
}

void ContourWindow::fireupSmoothContourGenerator()
{
    AbstractContourGenerator* generator = new SmoothContourGenerator();
    generator->setPolyData(m_polyData);
    generator->setNumberOfContours(m_numberOfContours);
    generator->setGenerateLabels(m_isLabelVisible);
    generator->setGenerateLines(m_isContourLineVisible);
    generator->setAutoDelete(false);
    connect(generator, SIGNAL(finished()), this, SLOT(onContourGeneratorFinished()));
    QThreadPool::globalInstance()->start(generator);

    setTriggerWidgetsEnabled(false);
}

void ContourWindow::fireupBandedContourGenerator()
{
    AbstractContourGenerator* generator = new BandedContourGenerator();
    generator->setPolyData(m_polyData);
    generator->setNumberOfContours(m_numberOfContours);
    generator->setGenerateLabels(m_isLabelVisible);
    generator->setGenerateLines(m_isContourLineVisible);
    generator->setAutoDelete(false);
    connect(generator, SIGNAL(finished()), this, SLOT(onContourGeneratorFinished()));
    QThreadPool::globalInstance()->start(generator);

    setTriggerWidgetsEnabled(false);
}

void ContourWindow::showContourLabels(bool show)
{
    if (!show && !m_labelActor)
        return;

    if (!show && m_labelActor) {
        m_labelActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    if (!m_polyData)
        return;

    if (m_isBandedContour) {
        fireupBandedContourGenerator();
//        createBandedContourLabels();
    } else {
        fireupSmoothContourGenerator();
//        createSmoothContourLabels();
    }
}

void ContourWindow::setScalars(double min, double max)
{
    m_scalar[0] = qMin(min, max);
    m_scalar[1] = qMax(min, max);
}

double ContourWindow::scalarMin() const
{
    return m_scalar[0];
}

double ContourWindow::scalarMax() const
{
    return m_scalar[1];
}

void ContourWindow::createSmoothContour()
{
    VTK_CREATE(vtkDelaunay2D, del);
    del->SetInputData(m_polyData);
    del->Update();

    double bounds[6];
    del->GetOutput()->GetBounds(bounds);
    double zMin = bounds[4];
    double zMax = bounds[5];
    setScalars(zMin, zMax);

    double xRange = bounds[1] - bounds[0];
    double yRange = bounds[3] - bounds[2];
    double zRange = bounds[4] - bounds[5];

    // add plane
    VTK_CREATE(vtkPlaneSource, planeSource);
    planeSource->SetXResolution(xRange);
    planeSource->SetYResolution(yRange);
    planeSource->SetOrigin(bounds[0], bounds[2], bounds[4]);
    planeSource->SetPoint1(bounds[1], bounds[2], bounds[4]);
    planeSource->SetPoint2(bounds[0], bounds[3], bounds[4]);
    planeSource->Update();

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(0.667, 0.0);
    lut->SetNumberOfColors(m_numberOfContours);
    lut->SetTableRange(bounds[4], bounds[5]);
    lut->Build();

    VtkUtils::vtkInitOnce(m_scalarBar);
    m_scalarBar->SetTitle("Scalar");
    m_scalarBar->SetLookupTable(lut);
    m_scalarBar->SetNumberOfLabels(m_numberOfContours);
    m_vtkWidget->defaultRenderer()->AddActor(m_scalarBar);

    VtkUtils::vtkInitOnce(m_cubeAxes);
    m_cubeAxes->SetInputData(planeSource->GetOutput());
    m_cubeAxes->SetZAxisVisibility(0);
    m_cubeAxes->SetFontFactor(1.0);
    m_cubeAxes->SetFlyModeToNone();
    m_cubeAxes->SetShowActualBounds(1);
    m_cubeAxes->SetNumberOfLabels(m_numberOfLabels);
    m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());

    m_vtkWidget->defaultRenderer()->AddViewProp(m_cubeAxes);

    // curve on the plane
    vtkPolyData* surfaceOutput = del->GetOutput();

    VTK_CREATE(vtkDoubleArray, planeScalars);
    planeScalars->SetNumberOfComponents(1);
    planeScalars->SetName("Isovalues");

    VTK_CREATE(vtkPolyData, customPlanePolyData);
    VTK_CREATE(vtkPoints, customPoints);
    double pos[3];
    int numOfPoints = surfaceOutput->GetNumberOfPoints();
    for (int i = 0; i < numOfPoints; i++) {
        surfaceOutput->GetPoint(i, pos);
        //
        customPoints->InsertPoint(i, pos[0], pos[1], bounds[4]);
        planeScalars->InsertNextTuple1(pos[2]);
    }

    customPlanePolyData->SetPoints(customPoints);
    customPlanePolyData->GetPointData()->SetScalars(planeScalars);

    VtkUtils::vtkInitOnce(m_planeDel);
    m_planeDel->SetOffset(qMax(xRange, qMax(yRange, zRange)));
    m_planeDel->SetInputData(customPlanePolyData);
    m_planeDel->Update();

    VTK_CREATE(vtkContourFilter, contourFilter);
    contourFilter->SetInputData(m_planeDel->GetOutput());
    contourFilter->GenerateValues(m_numberOfContours, bounds[4], bounds[5]);
    contourFilter->Update();

    VTK_CREATE(vtkPolyDataMapper, contourLinesMapper);
    contourLinesMapper->SetInputConnection(contourFilter->GetOutputPort());
    contourLinesMapper->SetScalarRange(bounds[4], bounds[5]);
    contourLinesMapper->SetScalarModeToUseCellData();
    contourLinesMapper->ScalarVisibilityOn();

    VtkUtils::vtkInitOnce(m_contourLineActor);
    m_contourLineActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
    m_contourLineActor->SetMapper(contourLinesMapper);

    // default to invisible
    m_contourLineActor->SetVisibility(m_isContourLineVisible);

    // surface
    VTK_CREATE(vtkPolyDataMapper, planeSurfaceMapper);
    planeSurfaceMapper->SetInputData(m_planeDel->GetOutput());
    planeSurfaceMapper->SetScalarRange(zMin, zMax);
    planeSurfaceMapper->SetLookupTable(lut);

    VtkUtils::vtkInitOnce(m_contourActor);
    m_contourActor->SetMapper(planeSurfaceMapper);
    m_contourActor->GetProperty()->SetInterpolationToPhong();

    m_vtkWidget->defaultRenderer()->AddActor(m_contourLineActor);
    m_vtkWidget->defaultRenderer()->AddActor(m_contourActor);
    m_vtkWidget->update();
    m_vtkWidget->defaultRenderer()->ResetCamera();

    // if use line widget then update its geometry
    if (m_lineWidget && m_lineWidget->GetEnabled())
        m_lineWidget->PlaceWidget(bounds);

    if (m_labelActor && m_labelActor->GetVisibility())
        showContourLabels(true);
}

void ContourWindow::createBandedContour()
{
    VTK_CREATE(vtkDelaunay2D, del);
    del->SetInputData(m_polyData);
    del->Update();

    double bounds[6];
    del->GetOutput()->GetBounds(bounds);
    double zMin = bounds[4];
    double zMax = bounds[5];
    double xRange = bounds[1] - bounds[0];
    double yRange = bounds[3] - bounds[2];
    double zRange = bounds[5] - bounds[4];
    setScalars(zMin, zMax);

    // add plane
    VTK_CREATE(vtkPlaneSource, planeSource);
    planeSource->SetXResolution(bounds[1] - bounds[0]);
    planeSource->SetYResolution(bounds[3] - bounds[2]);
    planeSource->SetOrigin(bounds[0], bounds[2], bounds[4]);
    planeSource->SetPoint1(bounds[1], bounds[2], bounds[4]);
    planeSource->SetPoint2(bounds[0], bounds[3], bounds[4]);
    planeSource->Update();

    // carve on the plane
    vtkPolyData* surfaceOutput = del->GetOutput();
    VtkUtils::vtkInitOnce(m_cubeAxes);
    m_cubeAxes->SetInputData(surfaceOutput);
    m_cubeAxes->SetZAxisVisibility(0);
    m_cubeAxes->SetFontFactor(1.0);
    m_cubeAxes->SetFlyModeToNone();
    m_cubeAxes->SetShowActualBounds(1);
    m_cubeAxes->SetNumberOfLabels(m_numberOfLabels);
    m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());

    VTK_CREATE(vtkDoubleArray, planeScalars);
    planeScalars->SetNumberOfComponents(1);
    planeScalars->SetName("Isovalues");

    VTK_CREATE(vtkPolyData, customPlanePolyData);
    VTK_CREATE(vtkPoints, customPoints);
    double pos[3];
    for (int i = 0; i < surfaceOutput->GetNumberOfPoints(); i++) {
        surfaceOutput->GetPoint(i, pos);
        customPoints->InsertPoint(i, pos[0], pos[1], zMin);
        planeScalars->InsertNextTuple1(pos[2]);
    }

    customPlanePolyData->SetPoints(customPoints);
    customPlanePolyData->GetPointData()->SetScalars(planeScalars);

    VtkUtils::vtkInitOnce(m_planeDel);
    m_planeDel->SetOffset(qMax(xRange, qMax(yRange, zRange)));
    m_planeDel->SetInputData(customPlanePolyData);
    m_planeDel->Update();

    vtkPolyData* planePolyData = m_planeDel->GetOutput();
//    setPolyData(del2->GetOutput());
    //
    double scalarRange[2];
    planePolyData->GetPointData()->GetScalars()->GetRange(scalarRange);

    VtkUtils::vtkInitOnce(m_cleanPolyData);

    double delta = (scalarRange[1] - scalarRange[0]) /
            static_cast<double> (m_numberOfContours - 1);

    // Keep the clippers alive
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersLo;
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersHi;
    VTK_CREATE(vtkAppendPolyData, appendFilledContours);

    VtkUtils::vtkInitOnce(m_labelPoints);
    VtkUtils::vtkInitOnce(m_labelScalars);

    m_labelPoints->Initialize();
    m_labelScalars->Initialize();

    for (int i = 0; i < m_numberOfContours; i++) {
        double valueLo = scalarRange[0] + static_cast<double> (i) * delta;
        double valueHi = scalarRange[0] + static_cast<double> (i + 1) * delta;
        clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersLo[i]->SetValue(valueLo);
        if (i == 0)
            clippersLo[i]->SetInputData(m_planeDel->GetOutput());
        else
            clippersLo[i]->SetInputConnection(clippersHi[i - 1]->GetOutputPort(1));
        clippersLo[i]->InsideOutOff();
        clippersLo[i]->Update();

        qDebug() << "value: " << valueHi;

        clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersHi[i]->SetValue(valueHi);
        clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
        clippersHi[i]->GenerateClippedOutputOn();
        clippersHi[i]->InsideOutOn();
        clippersHi[i]->Update();

        if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0)
            continue;

        VTK_CREATE(vtkFloatArray, cd);
        cd->SetNumberOfComponents(1);
        cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
        cd->FillComponent(0, valueLo);

        clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
        appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());

        // get label data
        vtkPolyData* hiPolyData = clippersHi[i]->GetClippedOutput();
        vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
        mapper->SetInputData(hiPolyData);
        mapper->Update();
        qDebug() << "num of points: " << hiPolyData->GetNumberOfPoints();

        m_labelScalars->InsertNextTuple1(valueHi);

        double midPoint[3];
        vtkPoints* points = hiPolyData->GetPoints();
        int numOfPoints = points->GetNumberOfPoints();

        vtkIdType midPointId = numOfPoints / 2;
        points->GetPoint(midPointId, midPoint);

//        qDebug() << "Center: " << midPoint[0] << "," << midPoint[1] << "," << midPoint[2];
        m_labelPoints->InsertNextPoint(midPoint);
    }

    m_cleanPolyData->SetInputConnection(appendFilledContours->GetOutputPort());

    //
    VTK_CREATE(vtkLookupTable, lut);
    lut->SetTableRange(zMin, zMax);
    lut->SetHueRange(0.6667, 0.0);
    lut->Build();

    VtkUtils::vtkInitOnce(m_scalarBar);
    m_scalarBar->SetTitle("Scalar");
    m_scalarBar->SetLookupTable(lut);
    m_scalarBar->SetNumberOfLabels(m_numberOfContours);
    m_vtkWidget->defaultRenderer()->AddActor(m_scalarBar);

    VTK_CREATE(vtkPolyDataMapper, contourMapper);
    contourMapper->SetInputConnection(m_cleanPolyData->GetOutputPort());
    contourMapper->SetScalarRange(zMin, zMax);
    contourMapper->SetScalarModeToUseCellData();
    contourMapper->SetLookupTable(lut);

    VtkUtils::vtkInitOnce(m_contourActor);
    m_contourActor->SetMapper(contourMapper);
    m_contourActor->GetProperty()->SetInterpolationToFlat();
    m_vtkWidget->defaultRenderer()->AddActor(m_contourActor);

    // contour lines
    if (m_isContourLineVisible) {
        VtkUtils::vtkInitOnce(m_contourLineActor);

        VTK_CREATE(vtkContourFilter, lineContour);
        lineContour->SetInputConnection(m_cleanPolyData->GetOutputPort());
        lineContour->GenerateValues(m_numberOfContours, zMin, zMax);

        VTK_CREATE(vtkPolyDataMapper, contourLineMapperer);
        contourLineMapperer->SetInputConnection(lineContour->GetOutputPort());
        contourLineMapperer->SetScalarRange(zMin, zMax);
        contourLineMapperer->ScalarVisibilityOff();

        VtkUtils::vtkInitOnce(m_contourLineActor);
        m_contourLineActor->SetMapper(contourLineMapperer);
        m_contourLineActor->SetVisibility(1);
        m_vtkWidget->defaultRenderer()->AddActor(m_contourLineActor);
    }

    // render labels
    if (m_isLabelVisible)
        createBandedContourLabels();

    m_vtkWidget->update();
}

void ContourWindow::createSmoothContourLabels()
{
    return;
    VTK_CREATE(vtkContourFilter, contourFilter);
    contourFilter->SetInputData(m_planeDel->GetOutput());
    contourFilter->GenerateValues(m_numberOfContours, scalarMin(), scalarMax());
    contourFilter->Update();

    vtkPolyData* contourOutput = contourFilter->GetOutput();

    vtkPoints *points = contourOutput->GetPoints();
    qDebug() << "num of points" << points->GetNumberOfPoints();
    vtkCellArray *cells = contourOutput->GetLines();
    qDebug() << "num of cells: " << cells->GetNumberOfCells();
    vtkDataArray *scalars = contourOutput->GetPointData()->GetScalars();
    qDebug() << "num of scalars: " << scalars->GetNumberOfTuples();

    VTK_CREATE(vtkPolyData, labelPolyData);
    VTK_CREATE(vtkPoints, labelPoints);
    VTK_CREATE(vtkDoubleArray, labelScalars);
    labelScalars->SetNumberOfComponents(1);
    labelScalars->SetName("Isovalues");

    vtkIdType *indices;
    vtkIdType numberOfPoints;
    unsigned int lineCount = 0;

    QSet<double> contourValues;
    for (cells->InitTraversal();
         cells->GetNextCell(numberOfPoints, indices);
         lineCount++) {
        // Compute the point id to hold the label
        // Mid point or a random point
        vtkIdType midPointId = indices[numberOfPoints / 2];
        midPointId = indices[static_cast<vtkIdType>(vtkMath::Random(0, numberOfPoints))];

        double midPoint[3];
        points->GetPoint(midPointId, midPoint);

        if (!contourValues.contains(scalars->GetTuple1(midPointId))) {
            labelPoints->InsertNextPoint(midPoint);
            labelScalars->InsertNextTuple1(scalars->GetTuple1(midPointId));
            contourValues.insert(scalars->GetTuple1(midPointId));
        }
    }

    labelPolyData->SetPoints(labelPoints);
    labelPolyData->GetPointData()->SetScalars(labelScalars);

    VtkUtils::vtkInitOnce(m_labeledDataMapper);
    m_labeledDataMapper->SetFieldDataName("Isovalues");
    m_labeledDataMapper->SetInputData(labelPolyData);
    m_labeledDataMapper->SetLabelModeToLabelScalars();
    m_labeledDataMapper->SetLabelFormat(m_labelFormat.toUtf8().data());
    m_labeledDataMapper->Update();

    VtkUtils::vtkInitOnce(m_labelActor);
    m_labelActor->SetMapper(m_labeledDataMapper);
    m_labelActor->SetVisibility(m_isLabelVisible);
    m_vtkWidget->defaultRenderer()->AddActor2D(m_labelActor);
    m_vtkWidget->update();
}

void ContourWindow::createBandedContourLabels()
{
    qDebug() << "banded contour labels: " << m_labelPoints->GetNumberOfPoints();
    qDebug() << "banded contour scalars: " << m_labelScalars->GetNumberOfTuples();
    VTK_CREATE(vtkPolyData, labelPolyData);
    labelPolyData->SetPoints(m_labelPoints);
    labelPolyData->GetPointData()->SetScalars(m_labelScalars);

    VtkUtils::vtkInitOnce(m_labeledDataMapper);
    m_labeledDataMapper->SetFieldDataName("Isovalues");
    m_labeledDataMapper->SetInputData(labelPolyData);
    m_labeledDataMapper->SetLabelModeToLabelScalars();
    m_labeledDataMapper->SetLabelFormat(m_labelFormat.toUtf8().data());
    m_labeledDataMapper->Update();

    VtkUtils::vtkInitOnce(m_labelActor);
    m_vtkWidget->defaultRenderer()->AddActor(m_labelActor);
    m_labelActor->SetMapper(m_labeledDataMapper);
    m_vtkWidget->update();
}

void ContourWindow::setTriggerWidgetsEnabled(bool enabled)
{
    m_ui->numOfContoursSpinBox->setEnabled(enabled);
    m_ui->demoSourcesCombo->setEnabled(enabled);
    m_ui->importFileButton->setEnabled(enabled);
    m_ui->bandedContourCheckBox->setEnabled(enabled);
    m_ui->showContourLineCheckBox->setEnabled(enabled);
    m_ui->labelGroupBox->setEnabled(enabled);
    m_ui->rowsSpinBox->setEnabled(enabled);
    m_ui->randomButton->setEnabled(enabled);
    m_ui->dataTable->setEnabled(enabled);
}

void ContourWindow::on_randomButton_clicked()
{
    // set demo source to none, cuz we use random data
    Utils::SignalBlocker sb(m_ui->demoSourcesCombo);
    m_ui->demoSourcesCombo->setCurrentIndex(0);

    m_tableModel->random();
    fireupModelToPointsConverter();
}

void ContourWindow::onModelToPointsConverterFinished()
{
    Utils::ModelToPointsConverter* mtpConverter = qobject_cast<Utils::ModelToPointsConverter*>(sender());

    VtkUtils::PointsToPolyDataConverter* newConverter = new VtkUtils::PointsToPolyDataConverter(mtpConverter->points());
    newConverter->setAutoDelete(false);
    connect(newConverter, SIGNAL(finished()), this, SLOT(onPointsToPolyDataConverterFinished()));
    QThreadPool::globalInstance()->start(newConverter);

    mtpConverter->deleteLater();
}

void ContourWindow::onColorChanged(const QColor& clr)
{
    if (m_labelColor != clr) {
        m_labelColor = clr;
        if (m_labelActor) {
            double clrArr[3];
            Utils::vtkColor(clr, clrArr);
            m_labelActor->GetProperty()->SetColor(clrArr);
            m_vtkWidget->update();
        }
    }
}

void ContourWindow::on_bandedContourCheckBox_toggled(bool checked)
{
    m_isBandedContour = checked;
    createContour();
}

void ContourWindow::on_showContourLineCheckBox_toggled(bool checked)
{
    if (!checked && m_contourLineActor) {
        m_contourLineActor->SetVisibility(checked);
        m_vtkWidget->update();
        return;
    }

    m_isContourLineVisible = checked;
    createContour();
}

void ContourWindow::on_importFileButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Import File"), ".", "All Files(*.*)");
    if (file.isEmpty())
        return;

    m_ui->importFileEdit->setText(file);
    fireupPointsReader(file);
}

void ContourWindow::on_pickPointCheckBox_toggled(bool checked)
{
    m_isPickPoint = checked;
}

void ContourWindow::on_numOfLabelsSpinBox_valueChanged(int arg1)
{
    if (m_cubeAxes) {
        m_numberOfLabels = arg1;
        m_cubeAxes->SetNumberOfLabels(arg1);
        m_vtkWidget->update();
    }
}

void ContourWindow::on_labelFormatEdit_textChanged(const QString &arg1)
{
    m_labelFormat = arg1;
    if (m_labeledDataMapper) {
        m_labeledDataMapper->SetLabelFormat(m_labelFormat.toUtf8().data());
        m_labeledDataMapper->Update();
        m_vtkWidget->update();
    }
}
