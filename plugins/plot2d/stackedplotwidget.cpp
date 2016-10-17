#include "stackedplotwidget.h"
#include "ui_tableplotwidget.h"

#include <utils/doublespinboxdelegate.h>
#include <utils/tablemodel.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkplot2dwidget.h>
#include <vtkutils/modeltovtktableconverter.h>

#include <vtkPlotStacked.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkColorSeries.h>

StackedPlotWidget::StackedPlotWidget(QWidget *parent) :
    PlotWidget(parent),
    m_ui(new Ui::TablePlotWidget)
{
    m_ui->setupUi(this);

    configTableView(m_ui->tableView);
    setWindowTitle(tr("Stacked Plot"));

    m_plotWidget = new VtkUtils::VtkPlot2DWidget(this);
    m_ui->verticalLayout->addWidget(m_plotWidget);

    m_ui->tableView->setModel(model());
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);
    m_ui->rowsSpinBox->setValue(sModel->rowCount());

    connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    fireupModelToVtkTableConverter();
}

StackedPlotWidget::~StackedPlotWidget()
{
    delete m_ui;
}

void StackedPlotWidget::onModelToVtkTableConverterFinished()
{
    VtkUtils::ModelToVtkTableConverter* converter = qobject_cast<VtkUtils::ModelToVtkTableConverter*>(sender());
    vtkTable* table = converter->table();

    int cols = table->GetNumberOfColumns();

    vtkChartXY* chart = vtkChartXY::SafeDownCast(m_plotWidget->chart());

    vtkPlotStacked *stack = 0;

    // Books
    if (!m_stacked) {
        m_stacked = vtkPlotStacked::SafeDownCast(chart->AddPlot(vtkChart::STACKED));
        m_stacked->SetUseIndexForXSeries(true);
    }

    m_stacked->SetInputData(table);

    QString columnName;
    for (int col = 1; col < cols; ++col) {
        columnName = QString("column#%1").arg(col + 1);
        m_stacked->SetInputArray(col, columnName.toUtf8().data());
    }

    VTK_CREATE(vtkColorSeries, colorSeries);
    colorSeries->SetColorScheme(vtkColorSeries::COOL);
    m_stacked->SetColorSeries(colorSeries);

    chart->SetForceAxesToBounds(true);
    table->Delete();
    converter->deleteLater();

    m_plotWidget->update();
}

void StackedPlotWidget::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVtkTableConverter();
}

void StackedPlotWidget::onModelRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}

void StackedPlotWidget::on_pushButton_clicked()
{
    model()->blockSignals(true);
    random();
    model()->blockSignals(false);
    emit model()->layoutChanged();
    fireupModelToVtkTableConverter();
}

void StackedPlotWidget::on_rowsSpinBox_valueChanged(int arg1)
{
    setRows(arg1);
}
