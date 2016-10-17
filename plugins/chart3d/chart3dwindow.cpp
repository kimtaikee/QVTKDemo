#include "chart3dwindow.h"
#include "ui_chart3dwindow.h"

#include <utils/signalblocker.h>
#include <utils/point3f.h>
#include <utils/tablemodel.h>
#include <utils/modeltopointsconverter.h>

#include <vtkutils/pointstopolydataconverter.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkActor.h>
#include <vtkScalarBarActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkRenderer.h>
#include <vtkOutlineFilter.h>
#include <vtkLineSource.h>
#include <vtkAppendPolyData.h>
#include <vtkPolygon.h>
#include <vtkLight.h>
#include <vtkSimplePointsReader.h>
#include <vtkSimplePointsWriter.h>

#include <QThreadPool>
#include <QFileDialog>
#include <QFileInfo>

Chart3DWindow::Chart3DWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::Chart3DWindow)
{
    createUi();

    m_ui->settingsGroupBox->hide();
    m_ui->colorButton->setEnabled(false);

    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    sb.addObject(m_ui->colorButton);

    m_ui->rowsSpinBox->setValue(DefaultRows);
    m_ui->colorButton->setCurrentColor(m_customColor);

    init();

    fireupModelToPointsConverter();
}

Chart3DWindow::~Chart3DWindow()
{
    delete m_ui;
}

void Chart3DWindow::createChart(vtkPolyData *polyData)
{
    if (!polyData)
        return;

    m_polyData = polyData;
    m_polyData->GetBounds(m_bounds);

    VtkUtils::vtkInitOnce(m_cubeAxes);
    m_cubeAxes->SetNumberOfLabels(DefaultLabels);
    m_cubeAxes->SetFontFactor(1.0);
    m_cubeAxes->SetFlyModeToNone();
    m_cubeAxes->SetCornerOffset(0);
    m_cubeAxes->SetShowActualBounds(1);
    m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());
    m_cubeAxes->SetInputData(m_polyData);
    m_vtkWidget->defaultRenderer()->AddViewProp(m_cubeAxes);

    // create outline
    VtkUtils::vtkInitOnce(m_outlineActor);

    VTK_CREATE(vtkOutlineFilter, outline);
    outline->SetInputData(polyData);

    VTK_CREATE(vtkPolyDataMapper, outlineMapper);
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    m_outlineActor->SetMapper(outlineMapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_outlineActor);

    // create scalar bar
    VtkUtils::vtkInitOnce(m_scalarBar);
    m_scalarBar->SetHeight(.8);
    m_scalarBar->SetWidth(.1);
    m_scalarBar->SetTitle("Value");

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(0.667, 0.0);
    lut->SetNumberOfColors(12);
    lut->SetTableRange(m_bounds[4], m_bounds[5]);
    lut->Build();

    m_scalarBar->SetLookupTable(lut);
    m_vtkWidget->defaultRenderer()->AddActor(m_scalarBar);

    showOrientationMarker();
    showAxisGrid();
    showAxisPlanes();
}

void Chart3DWindow::updateChart()
{
    // no impl
}

Utils::TableModel* Chart3DWindow::tableModel() const
{
    return m_tableModel;
}

vtkPolyData* Chart3DWindow::polyData() const
{
    return m_polyData;
}

qreal Chart3DWindow::xRange() const
{
    return m_bounds[1] - m_bounds[0];
}

qreal Chart3DWindow::yRange() const
{
    return m_bounds[3] - m_bounds[2];
}

qreal Chart3DWindow::zRange() const
{
    return m_bounds[5] - m_bounds[4];
}

void Chart3DWindow::on_randomButton_clicked()
{
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void Chart3DWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    m_tableModel->resize(3, arg1);
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void Chart3DWindow::on_coloringStyleCombo_currentIndexChanged(int index)
{
    m_useScalarColor = (index == 0);
    m_ui->colorButton->setEnabled(index != 0);

    if (m_useScalarColor)
        applyCustomColor();
    else
        applyScalarColor();
}

void Chart3DWindow::onModelToPointsConverterFinished()
{
    Utils::ModelToPointsConverter* converter = qobject_cast<Utils::ModelToPointsConverter*>(sender());
    m_points = converter->points();

    VtkUtils::PointsToPolyDataConverter* newConverter = new VtkUtils::PointsToPolyDataConverter(converter->points());
    newConverter->setAutoDelete(false);
    connect(newConverter, SIGNAL(finished()), this, SLOT(onPointsToPolyDataConverterFinished()));
    QThreadPool::globalInstance()->start(newConverter);

    // release mem
    converter->deleteLater();
}

void Chart3DWindow::onPointsToPolyDataConverterFinished()
{
    VtkUtils::PointsToPolyDataConverter* converter = qobject_cast<VtkUtils::PointsToPolyDataConverter*>(sender());
    createChart(converter->polyData());
    converter->deleteLater();
}

void Chart3DWindow::onColorChanged(const QColor& clr)
{
    m_customColor = clr;
    applyCustomColor();
}

void Chart3DWindow::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)

    fireupModelToPointsConverter();
}

void Chart3DWindow::init()
{
    m_tableModel = new Utils::TableModel(3, DefaultRows, this);
    m_tableModel->setHorizontalHeaderData(QVariantList() << "X" << "Y" << "Z");
    m_tableModel->random();
    m_ui->dataTable->setModel(m_tableModel);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    Utils::SignalBlocker sb(m_ui->planesColorButton);
    sb.addObject(m_ui->gridLinesColorButton);

    m_ui->planesColorButton->setCurrentColor(m_axisPlaneColor);
    m_ui->gridLinesColorButton->setCurrentColor(m_axisGridColor);

//    vtkLight* light = vtkLight::New();
//    light->SetAmbientColor(.3, .5, .7);
//    light->SetPosition(1, 1, 1);
//    m_vtkWidget->defaultRenderer()->AddLight(light);

    connect(m_tableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    connect(m_ui->colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
}

void Chart3DWindow::createUi()
{
    m_ui->setupUi(this);
}

void Chart3DWindow::showOrientationMarker(bool show)
{
    m_vtkWidget->showOrientationMarker(show);
}

void Chart3DWindow::showAxisPlanes(bool show)
{
    if (!show && !m_axisPlanesActor)
        return;

    if (!show && m_axisPlanesActor) {
        m_axisPlanesActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    int numOfPoints = 4;

    VTK_CREATE(vtkAppendPolyData, planesPolyData);

    double bounds[6];
    polyData()->GetBounds(bounds);

    double xMin = bounds[0];
    double xMax = bounds[1];
    double yMin = bounds[2];
    double yMax = bounds[3];
    double zMin = bounds[4];
    double zMax = bounds[5];

    // create xy plane
    {
        VTK_CREATE(vtkPolyData, pd);
        VTK_CREATE(vtkPoints, pots);
        pots->InsertNextPoint(xMin, yMin, zMin);
        pots->InsertNextPoint(xMax, yMin, zMin);
        pots->InsertNextPoint(xMax, yMax, zMin);
        pots->InsertNextPoint(xMin, yMax, zMin);
        pd->SetPoints(pots);

        VTK_CREATE(vtkPolygon, polygon);
        polygon->GetPointIds()->SetNumberOfIds(numOfPoints); //make a quad

        VTK_CREATE(vtkCellArray, polygons);
        vtkIdType p[4] = {0, 1, 2, 3};
        polygons->InsertNextCell(numOfPoints, p);

        // Add the polygon to a list of polygons
        pd->SetPolys(polygons);

        VTK_CREATE(vtkPolyData, polyData);
        polyData->SetPoints(pots);
        polyData->SetPolys(polygons);

        planesPolyData->AddInputData(polyData);
    }

    // create yz plane
    {
        VTK_CREATE(vtkPolyData, pd);
        VTK_CREATE(vtkPoints, pots);
        pots->InsertNextPoint(xMin, yMin, zMin);
        pots->InsertNextPoint(xMin, yMax, zMin);
        pots->InsertNextPoint(xMin, yMax, zMax);
        pots->InsertNextPoint(xMin, yMin, zMax);
        pd->SetPoints(pots);

        VTK_CREATE(vtkPolygon, polygon);
        polygon->GetPointIds()->SetNumberOfIds(numOfPoints); //make a quad

        VTK_CREATE(vtkCellArray, polygons);
        vtkIdType p[4] = {0, 1, 2, 3};
        polygons->InsertNextCell(numOfPoints, p);

        // Add the polygon to a list of polygons
        pd->SetPolys(polygons);

        VTK_CREATE(vtkPolyData, polyData);
        polyData->SetPoints(pots);
        polyData->SetPolys(polygons);
        planesPolyData->AddInputData(polyData);
    }

    // create xz plane
    {
        VTK_CREATE(vtkPolyData, pd);
        VTK_CREATE(vtkPoints, pots);
        pots->InsertNextPoint(xMin, yMax, zMin);
        pots->InsertNextPoint(xMax, yMax, zMin);
        pots->InsertNextPoint(xMax, yMax, zMax);
        pots->InsertNextPoint(xMin, yMax, zMax);
        pd->SetPoints(pots);

        VTK_CREATE(vtkPolygon, polygon);
        polygon->GetPointIds()->SetNumberOfIds(numOfPoints); //make a quad

        VTK_CREATE(vtkCellArray, polygons);
        vtkIdType p[4] = {0, 1, 2, 3};
        polygons->InsertNextCell(numOfPoints, p);

        // Add the polygon to a list of polygons
        pd->SetPolys(polygons);

        VTK_CREATE(vtkPolyData, polyData);
        polyData->SetPoints(pots);
        polyData->SetPolys(polygons);

        planesPolyData->AddInputData(polyData);
    }

    VTK_CREATE(vtkPolyDataMapper, planesMapper);
    planesMapper->SetInputConnection(planesPolyData->GetOutputPort());
    planesMapper->Update();

    VtkUtils::vtkInitOnce(m_axisPlanesActor);
    m_axisPlanesActor->SetVisibility(show);

    double vtkClr[3];
    Utils::vtkColor(m_axisPlaneColor, vtkClr);
    m_axisPlanesActor->GetProperty()->SetColor(vtkClr);
    m_axisPlanesActor->SetMapper(planesMapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_axisPlanesActor);
    m_vtkWidget->update();
}

void Chart3DWindow::showAxisGrid(bool show)
{
    if (!show && !m_axisGridActor)
        return;

    if (!show && m_axisGridActor) {
        m_axisGridActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    int markerCount = this->m_cubeAxes->GetNumberOfLabels() - 1;

    VTK_CREATE(vtkAppendPolyData, gridLinesPolyData);

    double bounds[6];
    polyData()->GetBounds(bounds);

    double xMin = bounds[0];
    double xMax = bounds[1];
    double yMin = bounds[2];
    double yMax = bounds[3];
    double zMin = bounds[4];
    double zMax = bounds[5];

    // create xy plane grids
    {
        double deltaX = (double)(xMax - xMin) / (markerCount);
        double initX = xMin;
        while (initX <= xMax) {
            VTK_CREATE(vtkLineSource, ls);
            ls->SetPoint1(initX, yMin, zMin);
            ls->SetPoint2(initX, yMax, zMin);
            ls->Update();
            gridLinesPolyData->AddInputData(ls->GetOutput());
            initX += deltaX;
        }

        double deltaY = (double)(yMax - yMin) / (markerCount);
        double initY = yMin;
        while (initY <= yMax) {
            VTK_CREATE(vtkLineSource, ls);
            ls->SetPoint1(xMin, initY, zMin);
            ls->SetPoint2(xMax, initY, zMin);
            ls->Update();
            gridLinesPolyData->AddInputData(ls->GetOutput());
            initY += deltaY;
        }
    }

    // draw yz plane grids
    {
        double deltaY = (yMax - yMin) / (markerCount);
        double initY = yMin;
        while (initY <= yMax) {
            VTK_CREATE(vtkLineSource, ls);
            ls->SetPoint1(xMin, initY, zMin);
            ls->SetPoint2(xMin, initY, zMax);
            ls->Update();
            gridLinesPolyData->AddInputData(ls->GetOutput());
            initY += deltaY;
        }

        double deltaZ = (zMax - zMin) / (markerCount);
        double initZ = zMin;
        while (initZ <= zMax) {
            VTK_CREATE(vtkLineSource, ls);
            ls->SetPoint1(xMin, yMin, initZ);
            ls->SetPoint2(xMin, yMax, initZ);
            ls->Update();
            gridLinesPolyData->AddInputData(ls->GetOutput());
            initZ += deltaZ;
        }
    }

    // draw xz plane grids
    {
        double deltaX = (xMax - xMin) / (markerCount);
        double initX = xMin;
        while (initX <= xMax) {
            VTK_CREATE(vtkLineSource, ls);
            ls->SetPoint1(initX, yMax, zMin);
            ls->SetPoint2(initX, yMax, zMax);
            ls->Update();
            gridLinesPolyData->AddInputData(ls->GetOutput());
            initX += deltaX;
        }

        double deltaZ = (zMax - zMin) / (markerCount);
        double initZ = zMin;
        while (initZ <= zMax) {
            VTK_CREATE(vtkLineSource, ls);
            ls->SetPoint1(xMin, yMax, initZ);
            ls->SetPoint2(xMax, yMax, initZ);
            ls->Update();
            gridLinesPolyData->AddInputData(ls->GetOutput());
            initZ += deltaZ;
        }
    }

    VtkUtils::vtkInitOnce(m_axisGridActor);

    gridLinesPolyData->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(gridLinesPolyData->GetOutputPort());
    m_axisGridActor->SetMapper(mapper);

    double vtkClr[3];
    Utils::vtkColor(m_axisGridColor, vtkClr);
    m_axisGridActor->GetProperty()->SetColor(vtkClr);
    m_axisGridActor->SetVisibility(show);

    m_vtkWidget->defaultRenderer()->AddActor(m_axisGridActor);
    m_vtkWidget->update();
}

void Chart3DWindow::setAxisGridColor(const QColor& clr)
{
    if (m_axisGridColor != clr) {
        m_axisGridColor = clr;
        showAxisGrid();
    }
}


void Chart3DWindow::fireupModelToPointsConverter()
{
    Utils::ModelToPointsConverter* converter = new Utils::ModelToPointsConverter(m_tableModel);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onModelToPointsConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

void Chart3DWindow::applyCustomColor()
{
    if (m_chartActor) {
        double clrArr[3];
        Utils::vtkColor(m_customColor, clrArr);
        m_chartActor->GetMapper()->ScalarVisibilityOff();
        m_chartActor->GetProperty()->SetColor(clrArr);
        m_vtkWidget->update();
    }
}

void Chart3DWindow::applyScalarColor()
{
    if (m_chartActor) {
        VTK_CREATE(vtkLookupTable, lut);
        lut->SetRange(m_bounds[4], m_bounds[5]); // z range
        lut->SetHueRange(0.6667, 0.0);
        lut->Build();

        m_chartActor->GetMapper()->SetLookupTable(lut);
        m_chartActor->GetMapper()->ScalarVisibilityOn();
        m_vtkWidget->update();
    }
}

void Chart3DWindow::on_showOutlineCheckBox_toggled(bool checked)
{
    if (m_outlineActor) {
        m_outlineActor->SetVisibility(checked);
        m_vtkWidget->update();
    }
}

void Chart3DWindow::on_numOfLabelsSpinBox_valueChanged(int arg1)
{
    if (m_cubeAxes) {
        m_cubeAxes->SetNumberOfLabels(arg1);
        m_vtkWidget->update();
        showAxisGrid();
    }
}

void Chart3DWindow::on_showGridLinesCheckBox_toggled(bool toggled)
{
    showAxisGrid(toggled);
}

void Chart3DWindow::on_showPlanesCheckBox_toggled(bool toggled)
{
    showAxisPlanes(toggled);
}

void Chart3DWindow::on_gridLinesColorButton_colorChanged(const QColor& clr)
{
    if (m_axisGridColor != clr) {
        m_axisGridColor = clr;
        showAxisGrid();
    }
}

void Chart3DWindow::on_planesColorButton_colorChanged(const QColor& clr)
{
    if (m_axisPlaneColor != clr) {
        m_axisPlaneColor = clr;
        showAxisPlanes();
    }
}

void Chart3DWindow::on_importFileButton_clicked()
{
    static QString previousDir = ".";
    QString importFile = QFileDialog::getOpenFileName(this, tr("Import File"), previousDir, "XYZ Files(*.xyz)");
    if (importFile.isEmpty())
        return;

    m_ui->importFileEdit->setText(importFile);

    QFileInfo fi(importFile);
    previousDir = fi.canonicalFilePath();

    VTK_CREATE(vtkSimplePointsReader, reader);
    reader->SetFileName(importFile.toUtf8().data());
    reader->Update();

    createChart(reader->GetOutput());
}

void Chart3DWindow::on_exportFileButton_clicked()
{
    static QString previousDir = ".";
    QString exportFile = QFileDialog::getSaveFileName(this, tr("Export File"), ".", "XYZ Files(*.xyz)");
    if (exportFile.isEmpty())
        return;

    m_ui->exportFileEdit->setText(exportFile);
    QFileInfo fi(exportFile);
    previousDir = fi.canonicalFilePath();

    VTK_CREATE(vtkSimplePointsWriter, writer);
    writer->SetFileName(exportFile.toUtf8().data());
    writer->SetInputData(polyData());
    writer->Write();
}

void Chart3DWindow::on_settingsGroupBox_toggled(bool toggled)
{
    m_configWidget->setVisible(toggled);
}





