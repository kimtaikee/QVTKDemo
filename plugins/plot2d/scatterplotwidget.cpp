#include "scatterplotwidget.h"
#include "ui_tableplotwidget.h"
#include "ui_scatterplotsettings.h"

#include <utils/tablemodel.h>
#include <utils/doublespinboxdelegate.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkplot2dwidget.h>
#include <vtkutils/modeltovtktableconverter.h>

#include <vtkPlot.h>
#include <vtkChartXY.h>
#include <vtkFloatArray.h>
#include <vtkTable.h>
#include <vtkPlotPoints.h>
#include <vtkColorSeries.h>

ScatterPlotWidget::ScatterPlotWidget(QWidget *parent) :
    PlotWidget(parent),
    m_ui(new Ui::TablePlotWidget)
{
    m_ui->setupUi(this);
    configTableView(m_ui->tableView);
    setWindowTitle(tr("Scatter Plot"));

    QWidget* settingsWidget = new QWidget(this);
    m_settingsUi = new Ui::ScatterPlotSettings;
    m_settingsUi->setupUi(settingsWidget);
    m_ui->settingsLayout->addWidget(settingsWidget);

    m_plotWidget = new VtkUtils::VtkPlot2DWidget(this);
    m_ui->verticalLayout->addWidget(m_plotWidget);

    m_ui->tableView->setModel(model());
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);
    m_ui->rowsSpinBox->setValue(sModel->rowCount());

    connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));

    fireupModelToVtkTableConverter();

    initConns();

#if 0
    vtkSmartPointer<vtkTable> table =
            vtkSmartPointer<vtkTable>::New();

    vtkSmartPointer<vtkFloatArray> arrX =
            vtkSmartPointer<vtkFloatArray>::New();
    arrX->SetName("X Axis");
    table->AddColumn(arrX);

    vtkSmartPointer<vtkFloatArray> arrC =
            vtkSmartPointer<vtkFloatArray>::New();
    arrC->SetName("Cosine");
    table->AddColumn(arrC);

    vtkSmartPointer<vtkFloatArray> arrS =
            vtkSmartPointer<vtkFloatArray>::New();
    arrS->SetName("Sine");
    table->AddColumn(arrS);

    vtkSmartPointer<vtkFloatArray> arrT =
            vtkSmartPointer<vtkFloatArray>::New();
    arrT->SetName("Sine - Cosine");
    table->AddColumn(arrT);

    // Test charting with a few more points...
    int numPoints = 40;
    float inc = 7.5 / (numPoints-1);
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
        table->SetValue(i, 0, i * inc);
        table->SetValue(i, 1, cos(i * inc) + 0.0);
        table->SetValue(i, 2, sin(i * inc) + 0.0);
        table->SetValue(i, 3, sin(i * inc) - cos(i * inc));
    }

    // Add multiple scatter plots, setting the colors etc
    vtkChartXY* chart = vtkChartXY::SafeDownCast(m_plotWidget->chart());
    vtkPlot *points = chart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 1);
    points->SetColor(0, 0, 0, 255);
    points->SetWidth(1.0);
    vtkPlotPoints::SafeDownCast(points)->SetMarkerStyle(vtkPlotPoints::CROSS);

    points = chart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 2);
    points->SetColor(0, 0, 0, 255);
    points->SetWidth(1.0);
    vtkPlotPoints::SafeDownCast(points)->SetMarkerStyle(vtkPlotPoints::PLUS);

    points = chart->AddPlot(vtkChart::POINTS);
    points->SetInputData(table, 0, 3);
    points->SetColor(0, 0, 255, 255);
    points->SetWidth(1.0);
    vtkPlotPoints::SafeDownCast(points)->SetMarkerStyle(vtkPlotPoints::CIRCLE);
#endif
}

ScatterPlotWidget::~ScatterPlotWidget()
{
    delete m_ui;
}

void ScatterPlotWidget::onModelToVtkTableConverterFinished()
{
    VtkUtils::ModelToVtkTableConverter* converter = qobject_cast<VtkUtils::ModelToVtkTableConverter*>(sender());
    vtkTable* table = converter->table();

    vtkChartXY* chart = vtkChartXY::SafeDownCast(m_plotWidget->chart());

    int cols = table->GetNumberOfColumns();

    vtkSmartPointer<vtkColorSeries> colorSeries = vtkColorSeries::New();
    colorSeries->SetColorScheme(vtkColorSeries::COOL);
    int colors = colorSeries->GetNumberOfColors();

    // first column as x
    for (int col = 1; col < cols; ++col) {
        if (!m_columnPlotMap.contains(col)) {
            vtkPlot* points = chart->AddPlot(vtkChart::POINTS);
            points->SetInputData(table, 0, col);

            auto clr = colorSeries->GetColor((col - 1) % colors);
            points->SetColor(clr.GetRed(), clr.GetGreen(), clr.GetBlue());
            points->SetWidth(1);

            vtkPlotPoints::SafeDownCast(points)->SetMarkerStyle(col % VTK_MARKER_UNKNOWN);

            m_columnPlotMap.insert(col, points);
        } else {
            vtkPlot* point = m_columnPlotMap.value(col);
            point->SetInputData(table, 0, col);
        }
    }

    chart->Update();
    m_plotWidget->update();

    converter->deleteLater();
    table->Delete();
}

void ScatterPlotWidget::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVtkTableConverter();
}

void ScatterPlotWidget::initConns()
{
    hookCheckBox(m_settingsUi->legendVisibleCheckBox);
    hookCheckBox(m_settingsUi->scalarVisibleCheckBox);
    hookDoubleSpinBox(m_settingsUi->markerSizeSpinBox);
    hookDoubleSpinBox(m_settingsUi->widthSpinBox);
    hookComboBox(m_settingsUi->markerStyleCombo);
}

void ScatterPlotWidget::on_pushButton_clicked()
{
    model()->blockSignals(true);
    random();
    model()->blockSignals(false);
    emit model()->layoutChanged();

    fireupModelToVtkTableConverter();
}

void ScatterPlotWidget::on_rowsSpinBox_valueChanged(int arg1)
{
    setRows(arg1);
}

void ScatterPlotWidget::onCheckBoxStateChanged(int state)
{
    QCheckBox* which = qobject_cast<QCheckBox*>(sender());
    bool checked = (state == Qt::Checked);
    QList<vtkPlotPoints*> points = plots<vtkPlotPoints>();

    if (which == m_settingsUi->scalarVisibleCheckBox) {
        foreach (vtkPlotPoints* point, points)
            point->SetScalarVisibility(checked);
    } else if (which == m_settingsUi->legendVisibleCheckBox) {
        foreach (vtkPlotPoints* point, points)
            point->SetLegendVisibility(checked);
    } else if (which == m_settingsUi->selectableCheckBox) {
        foreach (vtkPlotPoints* point, points)
            point->SetSelectable(checked);
    }

    m_plotWidget->update();
}

void ScatterPlotWidget::onDoubleSpinBoxValueChanged(double value)
{
    QDoubleSpinBox* which = qobject_cast<QDoubleSpinBox*>(sender());
    QList<vtkPlotPoints*> points = plots<vtkPlotPoints>();

    if (which == m_settingsUi->markerSizeSpinBox) {
        foreach (vtkPlotPoints* point, points)
            point->SetMarkerSize(value);
    } else if (which == m_settingsUi->widthSpinBox) {
        foreach (vtkPlotPoints* point, points)
            point->SetWidth(value);
    }

    m_plotWidget->update();
}

void ScatterPlotWidget::onComboBoxIndexChanged(int index)
{
    // Disable it if marker style is 'none'
    m_settingsUi->markerSizeSpinBox->setEnabled(index != 0);

    QList<vtkPlotPoints*> points = plots<vtkPlotPoints>();
    foreach (vtkPlotPoints* point, points)
        point->SetMarkerStyle(index);

    m_plotWidget->update();
}

void ScatterPlotWidget::onModelRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}


