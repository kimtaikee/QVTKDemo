#include "scaleaxeswindow.h"
#include "ui_scaleaxeswindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/pointstopolydataconverter.h>

#include <utils/tablemodel.h>
#include <utils/modeltopointsconverter.h>
#include <utils/signalblocker.h>

#include <vtkDelaunay2D.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkOutlineFilter.h>

#include <QThreadPool>
#include <QDebug>

ScaleAxesWindow::ScaleAxesWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ScaleAxesWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Scale Axes"));

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    m_tableModel = new Utils::TableModel(3, 100, this);

    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    sb.addObject(m_ui->xScaleSpinBox);
    sb.addObject(m_ui->yScaleSpinBox);
    sb.addObject(m_ui->zScaleSpinBox);
    sb.addObject(m_ui->useDataRangeCheckBox);

    m_ui->rowsSpinBox->setValue(100);
    m_ui->xScaleSpinBox->setValue(1.0);
    m_ui->yScaleSpinBox->setValue(1.0);
    m_ui->zScaleSpinBox->setValue(1.0);
    m_ui->useDataRangeCheckBox->setChecked(true);

    m_ui->tableView->setModel(m_tableModel);
    m_ui->tableView->setAlternatingRowColors(true);
    m_tableModel->random();

    fireupModelToPointsConverter();
}

ScaleAxesWindow::~ScaleAxesWindow()
{
    delete m_ui;
}

void ScaleAxesWindow::fireupModelToPointsConverter()
{
    Utils::ModelToPointsConverter* converter = new Utils::ModelToPointsConverter(m_tableModel);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onModelToPointsConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

void ScaleAxesWindow::renderSurface(vtkPolyData *data)
{
    if (!data) {
        qDebug() << "null polydata.";
        return;
    }

    VtkUtils::vtkInitOnce(m_polyData);
    m_polyData->DeepCopy(data);

    data->GetBounds(m_bounds);

    // create outline
    VtkUtils::vtkInitOnce(m_outlineActor);

    VTK_CREATE(vtkOutlineFilter, outline);
    outline->SetInputData(data);

    VTK_CREATE(vtkPolyDataMapper, outlineMapper);
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    m_outlineActor->SetMapper(outlineMapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_outlineActor);

    VtkUtils::vtkInitOnce(m_cubeAxes);
    m_cubeAxes->SetNumberOfLabels(10);
    m_cubeAxes->SetFontFactor(1.0);
    m_cubeAxes->SetFlyModeToNone();
    m_cubeAxes->SetCornerOffset(0);
    m_cubeAxes->SetShowActualBounds(1);
    m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());
    m_cubeAxes->SetInputData(data);
    m_vtkWidget->defaultRenderer()->AddViewProp(m_cubeAxes);

    VtkUtils::vtkInitOnce(m_surfaceActor);
    VtkUtils::vtkInitOnce(m_del);
    m_del->SetInputData(data);
    m_del->Update();

    double bounds[6];
    data->GetBounds(bounds);

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(0.6667, 0);
    lut->SetTableRange(bounds[4], bounds[5]);
    lut->Build();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(bounds[4], bounds[5]);
    mapper->SetInputConnection(m_del->GetOutputPort());

    m_surfaceActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_surfaceActor);
    m_vtkWidget->update();

    /// update ui
    // reset camera
    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void ScaleAxesWindow::printBounds()
{
    qDebug() << "-----------------";
    qDebug() << "| X: " << m_bounds[0] << "->" << m_bounds[1] << " |";
    qDebug() << "| Y: " << m_bounds[3] << "->" << m_bounds[2] << " |";
    qDebug() << "| Z: " << m_bounds[5] << "->" << m_bounds[4] << " |";
    qDebug() << "-----------------";
}

void ScaleAxesWindow::applyScales()
{
    m_surfaceActor->SetScale(m_ui->xScaleSpinBox->value(), m_ui->yScaleSpinBox->value(), m_ui->zScaleSpinBox->value());
    m_outlineActor->SetScale(m_ui->xScaleSpinBox->value(), m_ui->yScaleSpinBox->value(), m_ui->zScaleSpinBox->value());
    m_surfaceActor->GetBounds(m_bounds);

    if (!m_ui->useDataRangeCheckBox->isChecked()) {
        m_cubeAxes->SetInputData(nullptr);
        m_cubeAxes->SetBounds(m_bounds);
    }

    m_vtkWidget->update();

}

void ScaleAxesWindow::onModelToPointsConverterFinished()
{
    Utils::ModelToPointsConverter* converter = qobject_cast<Utils::ModelToPointsConverter*>(sender());

    VtkUtils::PointsToPolyDataConverter* newConverter = new VtkUtils::PointsToPolyDataConverter(converter->points());
    newConverter->setAutoDelete(false);
    connect(newConverter, SIGNAL(finished()), this, SLOT(onPointsToPolyDataConverterFinished()));
    QThreadPool::globalInstance()->start(newConverter);

    // release mem
    converter->deleteLater();
}

void ScaleAxesWindow::onPointsToPolyDataConverterFinished()
{
    VtkUtils::PointsToPolyDataConverter* converter = qobject_cast<VtkUtils::PointsToPolyDataConverter*>(sender());
    renderSurface(converter->polyData());
    converter->deleteLater();
}

void ScaleAxesWindow::on_randomButton_clicked()
{
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void ScaleAxesWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    m_tableModel->resize(3, arg1);
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void ScaleAxesWindow::on_xScaleSpinBox_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    applyScales();
}

void ScaleAxesWindow::on_yScaleSpinBox_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    applyScales();
}

void ScaleAxesWindow::on_zScaleSpinBox_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    applyScales();
}

void ScaleAxesWindow::on_scalingCheckBox_toggled(bool checked)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetScaling(checked);
    m_vtkWidget->update();
}

void ScaleAxesWindow::on_showActualBoundsCheckBox_toggled(bool checked)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetShowActualBounds(checked);
    m_vtkWidget->update();
}

void ScaleAxesWindow::on_useRangesCheckBox_toggled(bool checked)
{
    if (!m_cubeAxes)
        return;

    m_cubeAxes->SetUseRanges(checked);
    m_cubeAxes->SetRanges(m_bounds);
    m_vtkWidget->update();
}

void ScaleAxesWindow::on_useDataRangeCheckBox_toggled(bool checked)
{
    if (checked) {
        m_cubeAxes->SetInputData(m_polyData);
    } else {
        m_cubeAxes->SetInputData(nullptr);
        m_cubeAxes->SetBounds(m_bounds);
    }

    m_vtkWidget->update();
}
