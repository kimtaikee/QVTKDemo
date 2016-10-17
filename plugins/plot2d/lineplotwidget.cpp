#include "lineplotwidget.h"
#include "ui_tableplotwidget.h"
#include "ui_lineplotsettings.h"

#include <utils/tablemodel.h>
#include <utils/doublespinboxdelegate.h>
#include <utils/signalblocker.h>

#include <vtkutils/modeltovtktableconverter.h>
#include <vtkutils/vtkplot2dwidget.h>

#include <vtkPlot.h>
#include <vtkChartXY.h>
#include <vtkFloatArray.h>
#include <vtkTable.h>
#include <vtkPlotLine.h>
#include <vtkColorSeries.h>
#include <vtkPlotLine.h>

#include <QThreadPool>
#include <QDebug>

LinePlotWidget::LinePlotWidget(QWidget *parent) :
    PlotWidget(parent),
    m_ui(new Ui::TablePlotWidget)
{
    m_ui->setupUi(this);
    m_settingsUi = new Ui::LinePlotSettings;

    QWidget* settingsWidget = new QWidget(this);
    m_settingsUi->setupUi(settingsWidget);
    m_ui->settingsLayout->addWidget(settingsWidget);

    // config default settings
    m_settingsUi->polylineCheckBox->setChecked(true);
    m_settingsUi->widthSpinBox->setValue(1.0);
    m_settingsUi->markerSizeSpinBox->setEnabled(false);

    configTableView(m_ui->tableView);
    setWindowTitle(tr("Line Plot"));

    m_plotWidget = new VtkUtils::VtkPlot2DWidget(this);
    m_ui->verticalLayout->addWidget(m_plotWidget);

    m_ui->tableView->setModel(model());
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);
    m_ui->rowsSpinBox->setValue(sModel->rowCount());

    connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));

    fireupModelToVtkTableConverter();

    initConns();
#if 0
#define MY_IMPL
#ifdef MY_IMPL
    vtkTable* table = vtkTable::New();

    int rows = sModel->rowCount();
    int cols = sModel->columnCount();

    QString columnName;
    for (int i = 0; i < 3; ++i) {
        vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::New();
        columnName = QString("Column %1").arg(i + 1);
        arr->SetName(columnName.toUtf8().data()); // MUST set column name, or it will crash
        table->AddColumn(arr);
    }

    table->SetNumberOfRows(rows);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            qreal val = sModel->data(row, col);
            table->SetValue(row, col, val);
        }
    }

#else
//    vtkTable* table = convertModelToVtkTable();

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

    // Fill in the table with some example values
    int numPoints = 69;
    float inc = 7.5 / (numPoints-1);
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        table->SetValue(i, 0, i * inc);
        table->SetValue(i, 1, cos(i * inc));
        table->SetValue(i, 2, sin(i * inc));
    }
#endif
#endif

//    vtkChartXY* chart = vtkChartXY::SafeDownCast(m_plotWidget->chart());
//    vtkPlot *line = chart->AddPlot(vtkChart::LINE);
//    line->SetInputData(table, 0, 1);
//    line->SetColor(0, 255, 0, 255);
//    line->SetWidth(1.0);

//    line = chart->AddPlot(vtkChart::LINE);
//    line->SetInputData(table, 0, 2);
//    line->SetColor(255, 0, 0, 255);
//    line->SetWidth(5.0);
}

LinePlotWidget::~LinePlotWidget()
{
    delete m_ui;
}

void LinePlotWidget::onModelToVtkTableConverterFinished()
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
            vtkPlot* line = chart->AddPlot(vtkChart::LINE);
            line->SetInputData(table, 0, col);

            auto clr = colorSeries->GetColor((col - 1) % colors);
            line->SetColor(clr.GetRed(), clr.GetGreen(), clr.GetBlue());
            line->SetWidth(1);
            m_columnPlotMap.insert(col, line);
        } else {
            vtkPlot* line = m_columnPlotMap.value(col);
            line->SetInputData(table, 0, col);
        }
    }

//    chart->SetForceAxesToBounds(true);
    chart->Update();
    m_plotWidget->update();

    converter->deleteLater();
    table->Delete();
}

void LinePlotWidget::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVtkTableConverter();
}

void LinePlotWidget::initConns()
{
    hookCheckBox(m_settingsUi->polylineCheckBox);
    hookCheckBox(m_settingsUi->legendVisibleCheckBox);
    hookCheckBox(m_settingsUi->scalarVisibleCheckBox);
    hookDoubleSpinBox(m_settingsUi->markerSizeSpinBox);
    hookDoubleSpinBox(m_settingsUi->widthSpinBox);
    hookComboBox(m_settingsUi->markerStyleCombo);
}

void LinePlotWidget::on_pushButton_clicked()
{
    {
        Utils::SignalBlocker sb(model());
        random();
    }

    emit model()->layoutChanged();
    fireupModelToVtkTableConverter();
}

void LinePlotWidget::on_rowsSpinBox_valueChanged(int arg1)
{
    setRows(arg1);
}

void LinePlotWidget::onCheckBoxStateChanged(int state)
{
    QCheckBox* which = qobject_cast<QCheckBox*>(sender());
    bool checked = (state == Qt::Checked);
    QList<vtkPlotLine*> lines = plots<vtkPlotLine>();

    if (which == m_settingsUi->polylineCheckBox) {
        foreach (vtkPlotLine* line, lines)
            line->SetPolyLine(checked);
    } else if (which == m_settingsUi->scalarVisibleCheckBox) {
        foreach (vtkPlotLine* line, lines)
            line->SetScalarVisibility(checked);
    } else if (which == m_settingsUi->legendVisibleCheckBox) {
        foreach (vtkPlotLine* line, lines)
            line->SetLegendVisibility(checked);
    } else if (which == m_settingsUi->selectableCheckBox) {
        foreach (vtkPlotLine* line, lines)
            line->SetSelectable(checked);
    }

    m_plotWidget->update();
}

void LinePlotWidget::onDoubleSpinBoxValueChanged(double value)
{
    QDoubleSpinBox* which = qobject_cast<QDoubleSpinBox*>(sender());
    QList<vtkPlotLine*> lines = plots<vtkPlotLine>();

    if (which == m_settingsUi->markerSizeSpinBox) {
        foreach (vtkPlotLine* line, lines)
            line->SetMarkerSize(value);
    } else if (which == m_settingsUi->widthSpinBox) {
        foreach (vtkPlotLine* line, lines)
            line->SetWidth(value);
    }

    m_plotWidget->update();
}

void LinePlotWidget::onComboBoxIndexChanged(int index)
{
    // Disable it if marker style is 'none'
    m_settingsUi->markerSizeSpinBox->setEnabled(index != 0);

    QList<vtkPlotLine*> lines = plots<vtkPlotLine>();
    foreach (vtkPlotLine* line, lines)
        line->SetMarkerStyle(index);

    m_plotWidget->update();
}

void LinePlotWidget::onModelRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}

