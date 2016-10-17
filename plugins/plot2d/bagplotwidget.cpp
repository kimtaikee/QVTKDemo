#include "bagplotwidget.h"
#include "ui_tableplotwidget.h"

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
#include <vtkPlotBag.h>

#include <QThreadPool>
#include <QDebug>

BagPlotWidget::BagPlotWidget(QWidget *parent) :
    PlotWidget(parent),
    m_ui(new Ui::TablePlotWidget)
{
    m_ui->setupUi(this);
    configTableView(m_ui->tableView);
    setWindowTitle(tr("Bag Plot"));

    m_plotWidget = new VtkUtils::VtkPlot2DWidget(this);
    m_ui->verticalLayout->addWidget(m_plotWidget);

    m_ui->tableView->setModel(model());
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);
    m_ui->rowsSpinBox->setValue(sModel->rowCount());

    connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));

    fireupModelToVtkTableConverter();
}

BagPlotWidget::~BagPlotWidget()
{
    delete m_ui;
}

void BagPlotWidget::onModelToVtkTableConverterFinished()
{
    VtkUtils::ModelToVtkTableConverter* converter = qobject_cast<VtkUtils::ModelToVtkTableConverter*>(sender());
    vtkTable* table = converter->table();

    vtkChartXY* chart = vtkChartXY::SafeDownCast(m_plotWidget->chart());

    int cols = table->GetNumberOfColumns();

    vtkSmartPointer<vtkColorSeries> colorSeries = vtkColorSeries::New();
    colorSeries->SetColorScheme(vtkColorSeries::COOL);
    int colors = colorSeries->GetNumberOfColors();

    if (!m_plotBag) {
        m_plotBag = vtkPlotBag::New();
        chart->AddPlot(m_plotBag);
        m_plotBag->SetWidth(1);
    }

    m_plotBag->SetInputData(table, 0, 1, 2);

    chart->SetForceAxesToBounds(true);
    chart->Update();
    m_plotWidget->update();

    converter->deleteLater();
    table->Delete();
}

void BagPlotWidget::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVtkTableConverter();
}

void BagPlotWidget::onModelRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}

void BagPlotWidget::on_pushButton_clicked()
{
    model()->blockSignals(true);
    random();
    model()->blockSignals(false);
    emit model()->layoutChanged();
    fireupModelToVtkTableConverter();
}

void BagPlotWidget::on_rowsSpinBox_valueChanged(int arg1)
{
    setRows(arg1);
}

