#include "barplotwidget.h"
#include "ui_tableplotwidget.h"
#include "ui_barplotsettings.h"

#include <utils/tablemodel.h>
#include <utils/doublespinboxdelegate.h>
#include <utils/utils.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkplot2dwidget.h>
#include <vtkutils/modeltovtktableconverter.h>

#include <vtkPlot.h>
#include <vtkChartXY.h>
#include <vtkFloatArray.h>
#include <vtkTable.h>
#include <vtkIntArray.h>
#include <vtkColorSeries.h>
#include <vtkPlotBar.h>

// Monthly circulation data
static int data_2008[] = {10822, 10941, 9979, 10370, 9460, 11228, 15093, 12231, 10160, 9816, 9384, 7892};
static int data_2009[] = {9058, 9474, 9979, 9408, 8900, 11569, 14688, 12231, 10294, 9585, 8957, 8590};
static int data_2010[] = {9058, 10941, 9979, 10270, 8900, 11228, 14688, 12231, 10160, 9585, 9384, 8590};

BarPlotWidget::BarPlotWidget(QWidget *parent) :
    PlotWidget(parent),
    m_ui(new Ui::TablePlotWidget)
{
    m_ui->setupUi(this);

    m_settingsUi = new Ui::BarPlotSettings;
    QWidget* settingsWidget = new QWidget(this);
    m_settingsUi->setupUi(settingsWidget);
    m_ui->settingsLayout->addWidget(settingsWidget);

    configTableView(m_ui->tableView);
    setWindowTitle(tr("Bar Plot"));

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
    VTK_CREATE(vtkTable, table);

    VTK_CREATE(vtkIntArray, arrMonth);
    arrMonth->SetName("Month");
    table->AddColumn(arrMonth);

    VTK_CREATE(vtkIntArray, arr2008);
    arr2008->SetName("2008");
    table->AddColumn(arr2008);

    VTK_CREATE(vtkIntArray, arr2009);
    arr2009->SetName("2009");
    table->AddColumn(arr2009);

    VTK_CREATE(vtkIntArray, arr2010);
    arr2010->SetName("2010");
    table->AddColumn(arr2010);

    table->SetNumberOfRows(12);
    for (int i = 0; i < 12; i++)
    {
        table->SetValue(i,0,i+1);
        table->SetValue(i,1,data_2008[i]);
        table->SetValue(i,2,data_2009[i]);
        table->SetValue(i,3,data_2010[i]);
    }
#else
    vtkTable* table = convertModelToVtkTable();


#endif
    // Add multiple line plots, setting the colors etc
    vtkPlot *line = 0;

    vtkChartXY* chart = vtkChartXY::SafeDownCast(m_plotWidget->chart());
    chart->SetAutoAxes(true);
    chart->SetTitle("Bar Title");
    chart->SetShowLegend(true);
    chart->SetAutoSize(true);
    chart->SetRightBorder(2);

    line = chart->AddPlot(vtkChart::BAR);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);

    line = chart->AddPlot(vtkChart::BAR);
    line->SetInputData(table, 0, 2);
    line->SetColor(255, 0, 0, 255);

    line = chart->AddPlot(vtkChart::BAR);
    line->SetInputData(table, 0, 3);
    line->SetColor(0, 0, 255, 255);
#endif
}

BarPlotWidget::~BarPlotWidget()
{
    delete m_ui;
}

void BarPlotWidget::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVtkTableConverter();
}

void BarPlotWidget::onModelToVtkTableConverterFinished()
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
            vtkPlot* line = chart->AddPlot(vtkChart::BAR);
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

    m_plotWidget->update();

    converter->deleteLater();
    table->Delete();
}


void BarPlotWidget::on_pushButton_clicked()
{
    model()->blockSignals(true);
    random();
    model()->blockSignals(false);
    emit model()->layoutChanged();
    fireupModelToVtkTableConverter();
}

void BarPlotWidget::initConns()
{
    hookDoubleSpinBox(m_settingsUi->widthSpinBox);
    hookDoubleSpinBox(m_settingsUi->offsetSpinBox);
    hookCheckBox(m_settingsUi->scalarVisibleCheckBox);
    hookComboBox(m_settingsUi->orientationCombo);
}

void BarPlotWidget::onCheckBoxStateChanged(int state)
{
    QCheckBox* which = qobject_cast<QCheckBox*>(sender());
    bool checked = (state == Qt::Checked);

    QList<vtkPlotBar*> bars = plots<vtkPlotBar>();

    if (which == m_settingsUi->scalarVisibleCheckBox) {
        foreach (vtkPlotBar* bar, bars)
            bar->SetScalarVisibility(checked);
    }

    m_plotWidget->update();
}

void BarPlotWidget::onDoubleSpinBoxValueChanged(double value)
{
    QDoubleSpinBox* which = qobject_cast<QDoubleSpinBox*>(sender());
    QList<vtkPlotBar*> bars = plots<vtkPlotBar>();
    if (which == m_settingsUi->widthSpinBox) {
        foreach (vtkPlotBar* bar, bars)
            bar->SetWidth(value);
    } else if (which == m_settingsUi->offsetSpinBox) {
        foreach (vtkPlotBar* bar, bars)
            bar->SetOffset(value);
    }

    m_plotWidget->update();
}

void BarPlotWidget::onComboBoxIndexChanged(int index)
{
    QComboBox* which = qobject_cast<QComboBox*>(sender());
    QList<vtkPlotBar*> bars = plots<vtkPlotBar>();
    if (which == m_settingsUi->orientationCombo) {
        foreach (vtkPlotBar* bar, bars)
            bar->SetOrientation(index);
    }

    m_plotWidget->update();
}

void BarPlotWidget::onModelRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}

void BarPlotWidget::on_rowsSpinBox_valueChanged(int arg1)
{
    setRows(arg1);
}
