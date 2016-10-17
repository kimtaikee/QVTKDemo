#include "rendersurface.h"
#include "ui_rendersurface.h"

#include <utils/utils.h>
#include <utils/pointsreader.h>
#include <utils/tablemodel.h>
#include <utils/signalblocker.h>
#include <utils/pointstomodelconverter.h>
#include <utils/modeltopointsconverter.h>

#include <vtkutils/pointstopolydataconverter.h>
#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDelaunay2D.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkRenderer.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkOutlineFilter.h>
#include <vtkProperty.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkProperty2D.h>
#include <vtkAxisActor2D.h>
#include <vtkContourFilter.h>
#include <vtkLabeledDataMapper.h>
#include <vtkMath.h>
#include <vtkExtractEdges.h>
#include <vtkTubeFilter.h>
#include <vtkCubeAxesActor.h>

#include <QMenu>
#include <QAction>
#include <QThreadPool>
#include <QFileDialog>
#include <QDebug>

RenderSurface::RenderSurface(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::RenderSurface)
{
    m_ui->setupUi(this);
    init();
}

void RenderSurface::init()
{
    setWindowTitle(tr("Render Surface"));
    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_sourcesMenu = new QMenu(this);

    QStringList fileList = QStringList() << "ackley" << "data" << "demo" << "griewank"
                                         << "pit" << "schaffer";

    QActionGroup* actionGroup = new QActionGroup(this);
    foreach (const QString& file, fileList) {
        QAction* act = new QAction(file, this);
        act->setCheckable(true);
        actionGroup->addAction(act);
        connect(act, SIGNAL(triggered()), this, SLOT(onSourceActionTriggered()));
        m_sourcesMenu->addAction(act);
    }

    m_ui->sourcesButton->setMenu(m_sourcesMenu);

    m_tableModel = new Utils::TableModel(3, 30, this);
    m_tableModel->setHorizontalHeaderData(QVariantList() << "X" << "Y" << "Z");
    m_tableModel->random();

    connect(m_tableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this,
            SLOT(onTableModelDataChanged(QModelIndex,QModelIndex)));

    m_ui->tableView->setModel(m_tableModel);

    fireupModelToPointsConverter();

    initConnections();
    initSettings();
}

RenderSurface::~RenderSurface()
{
    delete m_ui;
}

void RenderSurface::onSourceActionTriggered()
{
    QAction* act = qobject_cast<QAction*>(sender());

    QString file = act->text();
    m_ui->sourcesButton->setText(file);
    fireupPointsReader(QString(":%1").arg(file));
}

void RenderSurface::onPointReaderFinished()
{
    Utils::PointsReader* reader = qobject_cast<Utils::PointsReader*>(sender());
    QList<Utils::Point3F> points = reader->points();

    renderSurfaceFromPoints(points);

    Utils::PointsToModelConverter* converter = new Utils::PointsToModelConverter(points, m_tableModel);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onPointsToModelConverterFinished()));
    QThreadPool::globalInstance()->start(converter);

    reader->deleteLater();
}

void RenderSurface::onPointsToModelConverterFinished()
{
    Utils::PointsToModelConverter* converter = qobject_cast<Utils::PointsToModelConverter*>(sender());
    emit m_tableModel->layoutChanged();
    converter->deleteLater();
}

void RenderSurface::onTableModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)

    fireupModelToPointsConverter();
}

void RenderSurface::onModelToPointsConverterFinished()
{
    Utils::ModelToPointsConverter* mtpConverter = qobject_cast<Utils::ModelToPointsConverter*>(sender());
    renderSurfaceFromPoints(mtpConverter->points());
    mtpConverter->deleteLater();
}

void RenderSurface::onColorChanged(const QColor &clr)
{
    Widgets::ColorPushButton* which = qobject_cast<Widgets::ColorPushButton*>(sender());

    if (which == m_ui->bgColorButton)
        m_vtkWidget->setBackgroundColor(clr);
    else if (which == m_ui->outlineButton)
        setOutlineColor(clr);
    else if (which == m_ui->contourLinesColorButton)
        setContourLinesColor(clr);
    else if (which == m_ui->labelsColorButton)
        setContourLabelsColor(clr);
    else if (which == m_ui->frameColorButton)
        setScalarBarFrameColor(clr);
    else if (which == m_ui->edgeColorButton)
        setEdgeColor(clr);
}

void RenderSurface::onPointsToPolyDataConverterFinished()
{
    VtkUtils::PointsToPolyDataConverter* ptpdConverter = qobject_cast<VtkUtils::PointsToPolyDataConverter*>(sender());

    VtkUtils::vtkInitOnce(m_surfaceActor);

    vtkPolyData* polydata = ptpdConverter->polyData();

    VtkUtils::vtkInitOnce(m_del);
    m_del->SetInputData(polydata);
    m_del->Update();

    double bounds[6];
    polydata->GetBounds(bounds);

    m_scalarMin = bounds[4];
    m_scalarMax = bounds[5];

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(0.6667, 0);
    lut->SetTableRange(m_scalarMin, m_scalarMax);
    lut->Build();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(m_scalarMin, m_scalarMax);
    mapper->SetInputConnection(m_del->GetOutputPort());

    m_surfaceActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_surfaceActor);
    m_vtkWidget->update();

    ptpdConverter->deleteLater();

    // update scalar bar's range
    if (m_scalarBar) {
        VTK_CREATE(vtkLookupTable, lut);
        lut->SetHueRange(0.6667, 0);
        lut->SetRange(m_scalarMin, m_scalarMax);
        lut->SetNumberOfColors(m_numOfScalarColors);
        lut->Build();
        m_scalarBar->SetLookupTable(lut);
    }

    // update contour
    if (m_contourActor && m_contourActor->GetVisibility())
        showContour(true);

    // update contour lines
    if (m_contourLinesActor && m_contourLinesActor->GetVisibility())
        showContourLines(true);

    if (m_contourLabelsActor && m_contourLabelsActor->GetVisibility())
        showContourLabels(true);

    /// update ui
    // reset camera
    m_vtkWidget->defaultRenderer()->ResetCamera();

    // prevent changing data
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(m_tableModel->rowCount());
}

void RenderSurface::initConnections()
{
    connect(m_ui->bgColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->outlineButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->contourLinesColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->labelsColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->frameColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->edgeColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_ui->cubeAxesColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setCubeAxesColor(QColor)));

    connect(m_ui->scalarBarGroupBox, SIGNAL(toggled(bool)), this, SLOT(onGroupBoxToggled(bool)));
    connect(m_ui->outlineGroupBox, SIGNAL(toggled(bool)), this, SLOT(onGroupBoxToggled(bool)));
    connect(m_ui->contourGroupBox, SIGNAL(toggled(bool)), this, SLOT(onGroupBoxToggled(bool)));
    connect(m_ui->cubeAxesGroupBox, SIGNAL(toggled(bool)), this, SLOT(onGroupBoxToggled(bool)));
}

void RenderSurface::initSettings()
{
    Utils::SignalBlocker sb(m_ui->bgColorButton);
    sb.addObject(m_ui->numOfColorsSpinBox);
    sb.addObject(m_ui->outlineButton);
    sb.addObject(m_ui->numOfContoursSpinBox);
    sb.addObject(m_ui->labelsColorButton);
    sb.addObject(m_ui->contourLinesColorButton);
    sb.addObject(m_ui->scalarBarOrientationComboBox);
    sb.addObject(m_ui->flyModeComboBox);
    sb.addObject(m_ui->numOfLabelsSpinBox);
    sb.addObject(m_ui->xLabelEdit);
    sb.addObject(m_ui->yLabelEdit);
    sb.addObject(m_ui->zLabelEdit);
    sb.addObject(m_ui->outlineOpacitySpinBox);
    sb.addObject(m_ui->showColorBarCheckBox);
    sb.addObject(m_ui->showTickLabelsCheckBox);
    sb.addObject(m_ui->outlineLineWidthSpinBox);
    sb.addObject(m_ui->frameColorButton);
    sb.addObject(m_ui->edgeColorButton);
    m_ui->edgeColorButton->setEnabled(false);

    m_ui->bgColorButton->setCurrentColor(Qt::black); // default to black
    m_ui->numOfColorsSpinBox->setValue(m_numOfScalarColors);
    m_ui->numOfColorsSpinBox->setEnabled(false);
    m_ui->outlineButton->setCurrentColor(Qt::white); // outline's default color
    m_ui->numOfContoursSpinBox->setValue(m_numOfContourLines);
    m_ui->labelsColorButton->setCurrentColor(Qt::white); // labels's default color
    m_ui->contourLinesColorButton->setCurrentColor(Qt::white); // contour line's default color
    m_ui->scalarBarOrientationComboBox->setCurrentIndex(1); // default to vertical
    m_ui->flyModeComboBox->setCurrentIndex(2); // default to none
    m_ui->numOfLabelsSpinBox->setValue(3); // default to 3
    m_ui->xLabelEdit->setText("X");
    m_ui->yLabelEdit->setText("Y");
    m_ui->zLabelEdit->setText("Z");
    m_ui->outlineOpacitySpinBox->setValue(1.0); // default to opaque
    m_ui->showColorBarCheckBox->setChecked(true);
    m_ui->showTickLabelsCheckBox->setChecked(true);
    m_ui->outlineLineWidthSpinBox->setValue(1); // default to 1
    m_ui->frameColorButton->setCurrentColor(Qt::white); // default to white
    m_ui->edgeColorButton->setCurrentColor(Qt::white); // default to white
    m_ui->cubeAxesColorButton->setCurrentColor(Qt::white); // default to white

    m_ui->frameColorButton->setEnabled(false);
    m_ui->frameWidthSpinBox->setEnabled(false);
}

void RenderSurface::renderSurfaceFromPoints(const QList<Utils::Point3F>& points)
{
    if (points.isEmpty())
        return;

    VtkUtils::PointsToPolyDataConverter* ptpdConverter = new VtkUtils::PointsToPolyDataConverter(points);
    ptpdConverter->setAutoDelete(false);
    connect(ptpdConverter, SIGNAL(finished()), this, SLOT(onPointsToPolyDataConverterFinished()));
    QThreadPool::globalInstance()->start(ptpdConverter);
}

void RenderSurface::fireupModelToPointsConverter()
{
    Utils::ModelToPointsConverter* mtpConverter = new Utils::ModelToPointsConverter(m_tableModel);
    mtpConverter->setAutoDelete(false);
    connect(mtpConverter, SIGNAL(finished()), this, SLOT(onModelToPointsConverterFinished()));
    QThreadPool::globalInstance()->start(mtpConverter);
}

void RenderSurface::fireupPointsReader(const QString& file)
{
    Utils::PointsReader* pointsReader = new Utils::PointsReader(file);
    pointsReader->setAutoDelete(false);
    connect(pointsReader, SIGNAL(finished()), this, SLOT(onPointReaderFinished()));
    QThreadPool::globalInstance()->start(pointsReader);
}

void RenderSurface::on_viewFitButton_clicked()
{
    m_vtkWidget->defaultRenderer()->ResetCamera();
    m_vtkWidget->update();
}

void RenderSurface::showOrientationMarker(bool show)
{
    m_vtkWidget->showOrientationMarker(show);
}

void RenderSurface::showScalarBar(bool show)
{
    // enable this option only when the scalar bar is visible
    m_ui->numOfColorsSpinBox->setEnabled(show);

    if (!m_scalarBar) {
        VTK_CREATE(vtkLookupTable, lut);
        lut->SetHueRange(0.6667, 0);
        lut->SetRange(m_scalarMin, m_scalarMax);
        lut->SetNumberOfColors(m_numOfScalarColors);
        lut->Build();

        m_scalarBar = vtkScalarBarActor::New();
        m_scalarBar->SetWidth(.1); // fraction of window width
        m_scalarBar->SetHeight(.8); // fraction of window height
        m_scalarBar->GetTitleTextProperty()->SetFontSize(3);
        m_scalarBar->GetTitleTextProperty()->SetBold(0);
        m_scalarBar->GetTitleTextProperty()->SetItalic(0);
        m_scalarBar->SetOrientationToVertical();
        m_scalarBar->SetTitle("Scalar");
        m_scalarBar->SetLookupTable(lut);

        m_vtkWidget->addActor(m_scalarBar, m_ui->bgColorButton->currentColor());
    }


    m_scalarBar->SetVisibility(show);
    m_vtkWidget->update();

    // config sb
#if 0
    double* clr = detail::Util::vtkColor(defaultScalarBarTextColor());
    m_scalarBar->GetProperty()->SetColor(clr);
    m_scalarBar->GetTitleTextProperty()->SetColor(clr);
    m_scalarBar->GetLabelTextProperty()->SetShadow(0);
    m_scalarBar->GetTitleTextProperty()->SetShadow(0);
#endif
}

void RenderSurface::setScalarBarColors(int num)
{
    if (m_numOfScalarColors == num)
        return;

    m_numOfScalarColors = num;

    if (m_scalarBar) {
        VTK_CREATE(vtkLookupTable, lut);
        lut->SetHueRange(0.6667, 0);
        lut->SetRange(m_scalarMin, m_scalarMax);
        lut->SetNumberOfColors(m_numOfScalarColors);
        lut->Build();
        m_scalarBar->SetLookupTable(lut);
        m_vtkWidget->update();
    }
}

void RenderSurface::showCubeAxes(bool show)
{
    if (!m_cubeAxes) {
        m_cubeAxes = vtkCubeAxesActor2D::New();
        m_cubeAxes->SetFontFactor(1.0);
        m_cubeAxes->SetFlyModeToNone();
        m_cubeAxes->SetShowActualBounds(1);
        m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());
        m_vtkWidget->addViewProp(m_cubeAxes);
    }

    m_cubeAxes->SetInputData(m_del->GetOutput());
    m_cubeAxes->SetVisibility(show);
    m_vtkWidget->update();
}

void RenderSurface::showOutline(bool show)
{
    if (!m_del)
        return;

    if (!show && m_outlineActor) {
        m_outlineActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    // display dataset's bounding box
    VTK_CREATE(vtkOutlineFilter, outline);
    outline->SetInputConnection(m_del->GetOutputPort());

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(outline->GetOutputPort());

    VtkUtils::vtkInitOnce(m_outlineActor);
    m_outlineActor->SetMapper(mapper);

    m_vtkWidget->defaultRenderer()->AddActor(m_outlineActor);

    m_outlineActor->SetVisibility(show);
    m_vtkWidget->update();
}

void RenderSurface::setOutlineColor(const QColor &clr)
{
    if (!m_outlineActor)
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);
    m_outlineActor->GetProperty()->SetColor(vtkClr);
    m_vtkWidget->update();
}

void RenderSurface::setCubeAxesColor(const QColor& clr)
{
    // TODO
    if (!m_cubeAxes)
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);
    m_cubeAxes->GetXAxisActor2D()->GetProperty()->SetColor(vtkClr);
    m_cubeAxes->GetYAxisActor2D()->GetProperty()->SetColor(vtkClr);
    m_cubeAxes->GetZAxisActor2D()->GetProperty()->SetColor(vtkClr);
    m_vtkWidget->update();
}

void RenderSurface::setContourLinesColor(const QColor& clr)
{
    if (!m_contourLinesActor)
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);
    qDebug() << "line color: " << vtkClr[0] << "," << vtkClr[1] << "," << vtkClr[2];
    m_contourLinesActor->GetProperty()->SetColor(vtkClr);
    m_vtkWidget->update();
}

void RenderSurface::setContourLabelsColor(const QColor& clr)
{
    if (!m_contourLabelsActor)
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);
    m_contourLabelsActor->GetProperty()->SetColor(vtkClr);
    m_vtkWidget->update();
}

void RenderSurface::setScalarBarFrameColor(const QColor& clr)
{
    if (!m_scalarBar)
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);
    vtkProperty2D* prop = m_scalarBar->GetFrameProperty();
    prop->SetColor(vtkClr);
    m_vtkWidget->update();
}

void RenderSurface::setEdgeColor(const QColor& clr)
{
    if (!m_edgeActor)
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);
    m_edgeActor->GetProperty()->SetColor(vtkClr);
    m_vtkWidget->update();
}

void RenderSurface::showContour(bool show)
{
    if (!m_del)
        return;

    if (!show && m_contourActor) {
        m_contourActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    vtkPolyData* polyData = m_del->GetOutput();

    double bounds[6];
    polyData->GetBounds(bounds);

    double xRange = bounds[1] - bounds[0];
    double yRange = bounds[3] - bounds[2];
    double zRange = bounds[5] - bounds[4];

    VTK_CREATE(vtkDoubleArray, planeScalars);
    planeScalars->SetNumberOfComponents(1);
    planeScalars->SetName("Isovalues");

    VTK_CREATE(vtkPolyData, contourPolyData);
    VTK_CREATE(vtkPoints, contourPoints);

    double pos[3];
    for (int i = 0; i < polyData->GetNumberOfPoints(); i++) {
        polyData->GetPoint(i, pos);
        contourPoints->InsertPoint(i, pos[0], pos[1], m_scalarMin);
        planeScalars->InsertNextTuple1(pos[2]);
    }

    contourPolyData->SetPoints(contourPoints);
    contourPolyData->GetPointData()->SetScalars(planeScalars);

    VTK_CREATE(vtkDelaunay2D, del2);
    del2->SetInputData(contourPolyData);
    del2->SetOffset(qMax(xRange, qMax(yRange, zRange)));
    del2->Update();

    VTK_CREATE(vtkSmoothPolyDataFilter, smoothFilter);
    smoothFilter->SetNumberOfIterations(100);
    smoothFilter->SetInputConnection(del2->GetOutputPort());

    VTK_CREATE(vtkLookupTable, colorLookupTable);
    colorLookupTable->SetTableRange(m_scalarMin, m_scalarMax);
    colorLookupTable->SetHueRange(0.6667, 0.0);
    colorLookupTable->Build();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(smoothFilter->GetOutputPort());
    mapper->SetLookupTable(colorLookupTable);
    mapper->SetScalarRange(m_scalarMin, m_scalarMax);

    VtkUtils::vtkInitOnce(m_contourActor);
    m_contourActor->SetMapper(mapper);
    m_contourActor->SetVisibility(1);

    m_vtkWidget->defaultRenderer()->AddActor(m_contourActor);
    m_vtkWidget->update();
}

void RenderSurface::showContourLines(bool show)
{
    if (!m_del)
        return;

    if (!show && m_contourLinesActor) {
        m_contourLinesActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    VTK_CREATE(vtkContourFilter, filter);
    filter->SetInputConnection(m_del->GetOutputPort());
    filter->GenerateValues(m_numOfContourLines, m_scalarMin, m_scalarMax);

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(filter->GetOutputPort());
    mapper->SetScalarVisibility(0); // only when this option is turned off, we could customize its color

    VtkUtils::vtkInitOnce(m_contourLinesActor);
    m_contourLinesActor->SetMapper(mapper);
    m_contourLinesActor->SetVisibility(1);

    m_vtkWidget->defaultRenderer()->AddActor(m_contourLinesActor);
    m_vtkWidget->update();
}

void RenderSurface::showContourLabels(bool show)
{
    if (!m_del)
        return;

    if (!show && m_contourLabelsActor) {
        m_contourLabelsActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    VTK_CREATE(vtkContourFilter, contour);
    contour->SetInputConnection(m_del->GetOutputPort());
    contour->GenerateValues(m_numOfContourLines, m_scalarMin, m_scalarMax);
    contour->Update();

    vtkPolyData* polyData = contour->GetOutput();
    vtkPoints *points = polyData->GetPoints();
    vtkCellArray *cells = polyData->GetLines();
    vtkDataArray *scalars = polyData->GetPointData()->GetScalars();

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

    VTK_CREATE(vtkLabeledDataMapper, labelMapper);
    labelMapper->SetFieldDataName("Isovalues");
    labelMapper->SetInputData(labelPolyData);
    labelMapper->SetLabelModeToLabelScalars();
    labelMapper->SetLabelFormat("%6.2f");

    VtkUtils::vtkInitOnce(m_contourLabelsActor);
    m_contourLabelsActor->SetMapper(labelMapper);
    m_contourLabelsActor->SetVisibility(show);

    m_vtkWidget->defaultRenderer()->AddActor2D(m_contourLabelsActor);
    m_vtkWidget->update();
}

void RenderSurface::showBandedContour(bool show)
{
    // TODO
}

void RenderSurface::showEdges(bool show)
{
    if (!m_del)
        return;

    if (!show && m_edgeActor) {
        m_edgeActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    VTK_CREATE(vtkExtractEdges, extractEdge);
    extractEdge->SetInputConnection(m_del->GetOutputPort());

//    VTK_CREATE(vtkTubeFilter, tube);
//    tube->SetInputConnection(extractEdge->GetOutputPort());

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(extractEdge->GetOutputPort());
    mapper->SetScalarVisibility(0);

    VtkUtils::vtkInitOnce(m_edgeActor);
    m_edgeActor->SetMapper(mapper);
    m_edgeActor->SetVisibility(show);
    m_vtkWidget->defaultRenderer()->AddActor(m_edgeActor);
    m_vtkWidget->update();
}

void RenderSurface::onGroupBoxToggled(bool on)
{
    QGroupBox* which = qobject_cast<QGroupBox*>(sender());

    if (which == m_ui->scalarBarGroupBox)
        showScalarBar(on);
    else if (which == m_ui->outlineGroupBox)
        showOutline(on);
    else if (which == m_ui->contourGroupBox)
        showContour(on);
    else if (which == m_ui->cubeAxesGroupBox)
        showCubeAxes(on);
}

void RenderSurface::on_showOriMarkerButton_toggled(bool checked)
{
    showOrientationMarker(checked);
}

void RenderSurface::on_showScalarBarButton_toggled(bool checked)
{
    showScalarBar(checked);
}

void RenderSurface::on_numOfColorsSpinBox_valueChanged(int arg1)
{
    setScalarBarColors(arg1);
}

void RenderSurface::on_showCubeAxesButton_toggled(bool checked)
{
    showCubeAxes(checked);
}

void RenderSurface::on_showOutlineButton_toggled(bool checked)
{
    showOutline(checked);
}

void RenderSurface::on_showContourButton_toggled(bool checked)
{
    showContour(checked);
}

void RenderSurface::on_randomButton_clicked()
{
    m_ui->sourcesButton->setText(tr("random"));
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void RenderSurface::on_rowsSpinBox_valueChanged(int arg1)
{
    m_tableModel->resize(m_tableModel->columnCount(), arg1);
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void RenderSurface::on_loadFileButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Load Data File"), ".", "All Files(*.*)");
    if (file.isEmpty())
        return;
    fireupPointsReader(file);
    m_ui->sourcesButton->setText(tr("File"));
}

void RenderSurface::on_checkBox_toggled(bool checked)
{
    showContourLines(checked);
}

void RenderSurface::on_showLabelsCheckBox_toggled(bool checked)
{
    showContourLabels(checked);
}

void RenderSurface::on_showContourLinesCheckBox_toggled(bool checked)
{
    showContourLines(checked);
}

void RenderSurface::on_numOfContoursSpinBox_valueChanged(int arg1)
{
    if (m_numOfContourLines != arg1) {
        m_numOfContourLines = arg1;

        if (m_contourLinesActor && m_contourLinesActor->GetVisibility())
            showContourLines(true);

        if (m_contourLabelsActor && m_contourLabelsActor->GetVisibility())
            showContourLabels(true);
    }
}

void RenderSurface::on_scalarBarOrientationComboBox_currentIndexChanged(int index)
{
    if (!m_scalarBar)
        return;

    m_scalarBar->SetOrientation(index);

    if (index == 0) {
        // 0 for horizontal
        m_scalarBar->SetWidth(.8);
        m_scalarBar->SetHeight(.1);
        m_scalarBar->SetPosition(0.1, 0.1);
    } else {
        // 1 for vertical
        m_scalarBar->SetWidth(.1);
        m_scalarBar->SetHeight(.8);
        m_scalarBar->SetPosition(0.85, 0.1);
    }

    m_vtkWidget->update();
}

void RenderSurface::on_flyModeComboBox_currentIndexChanged(int index)
{
    if (!m_cubeAxes)
        return;

    // 0 for outer edges
    // 1 for closest triad
    // 2 for none
    m_cubeAxes->SetFlyMode(index);
    m_vtkWidget->update();
}

void RenderSurface::on_numOfLabelsSpinBox_valueChanged(int arg1)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetNumberOfLabels(arg1);
    m_vtkWidget->update();
}

void RenderSurface::on_showXAxisCheckBox_toggled(bool checked)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetXAxisVisibility(checked);
    m_vtkWidget->update();
}

void RenderSurface::on_showYAxisCheckBox_toggled(bool checked)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetYAxisVisibility(checked);
    m_vtkWidget->update();
}

void RenderSurface::on_showZAxisCheckBox_toggled(bool checked)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetZAxisVisibility(checked);
    m_vtkWidget->update();
}

void RenderSurface::on_xLabelEdit_textChanged(const QString &arg1)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetXLabel(arg1.toUtf8().data());
    m_vtkWidget->update();
}

void RenderSurface::on_yLabelEdit_textChanged(const QString &arg1)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetYLabel(arg1.toUtf8().data());
    m_vtkWidget->update();
}

void RenderSurface::on_zLabelEdit_textChanged(const QString &arg1)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetZLabel(arg1.toUtf8().data());
    m_vtkWidget->update();
}

void RenderSurface::on_genFacesCheckBox_toggled(bool checked)
{
    if (!m_del)
        return;

    VTK_CREATE(vtkOutlineFilter, outline);
    outline->SetGenerateFaces(checked);
    outline->SetInputConnection(m_del->GetOutputPort());

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(outline->GetOutputPort());

    VtkUtils::vtkInitOnce(m_outlineActor);
    m_outlineActor->SetMapper(mapper);

    m_vtkWidget->defaultRenderer()->AddActor(m_outlineActor);
    m_vtkWidget->update();
}

void RenderSurface::on_outlineOpacitySpinBox_valueChanged(double arg1)
{
    if (!m_outlineActor)
        return;

    m_outlineActor->GetProperty()->SetOpacity(arg1);
    m_vtkWidget->update();
}

void RenderSurface::on_showFrameCheckBox_toggled(bool checked)
{
    if (!m_scalarBar)
        return;

    m_ui->frameColorButton->setEnabled(checked);
    m_ui->frameWidthSpinBox->setEnabled(checked);

    m_scalarBar->SetDrawFrame(checked);
    m_vtkWidget->update();
}

void RenderSurface::on_showColorBarCheckBox_toggled(bool checked)
{
    if (!m_scalarBar)
        return;

    m_scalarBar->SetDrawColorBar(checked);
    m_vtkWidget->update();
}

void RenderSurface::on_showTickLabelsCheckBox_toggled(bool checked)
{
    if (!m_scalarBar)
        return;

    m_scalarBar->SetDrawTickLabels(checked);
    m_vtkWidget->update();
}

void RenderSurface::on_frameWidthSpinBox_valueChanged(int arg1)
{
    if (!m_scalarBar)
        return;

    m_scalarBar->GetFrameProperty()->SetLineWidth(arg1);
    m_vtkWidget->update();
}

void RenderSurface::on_outlineLineWidthSpinBox_valueChanged(int arg1)
{
    if (!m_outlineActor)
        return;

    m_outlineActor->GetProperty()->SetLineWidth(arg1);
    m_vtkWidget->update();
}

void RenderSurface::on_showEdgeCheckBox_toggled(bool checked)
{
    m_ui->edgeColorButton->setEnabled(checked);
    showEdges(checked);
}

void RenderSurface::on_configGroupBox_toggled(bool arg1)
{
    m_ui->tableGroupBox->setVisible(arg1);
    m_ui->generalGroupBox->setVisible(arg1);
    m_ui->scalarBarGroupBox->setVisible(arg1);
    m_ui->outlineGroupBox->setVisible(arg1);
    m_ui->contourGroupBox->setVisible(arg1);
    m_ui->cubeAxesGroupBox->setVisible(arg1);

    if (!arg1)
        m_ui->configGroupBox->setFixedHeight(20);
    else
        m_ui->configGroupBox->setFixedHeight(200);
}

void RenderSurface::on_gradientCombo_activated(int index)
{
    Q_UNUSED(index);
    Widgets::GradientComboBox* gcb = qobject_cast<Widgets::GradientComboBox*>(sender());

    double hsv1[3];
    double hsv2[3];
    Utils::qColor2HSV(gcb->currentColor1(), hsv1);
    Utils::qColor2HSV(gcb->currentColor2(), hsv2);

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(hsv1[0], hsv2[0]);
    lut->SetSaturationRange(hsv1[1], hsv2[1]);
    lut->SetValueRange(hsv1[2], hsv2[2]);
    lut->SetTableRange(m_scalarMin, m_scalarMax);
    lut->SetNumberOfColors(m_numOfContourLines);
    lut->Build();

    if (m_surfaceActor)
        m_surfaceActor->GetMapper()->SetLookupTable(lut);

    if (m_scalarBar)
        m_scalarBar->SetLookupTable(lut);

    if (m_contourActor)
        m_contourActor->GetMapper()->SetLookupTable(lut);

    m_vtkWidget->update();

}
