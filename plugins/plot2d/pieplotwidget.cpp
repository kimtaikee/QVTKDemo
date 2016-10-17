#include "pieplotwidget.h"
#include "ui_tableplotwidget.h"

#include <utils/tablemodel.h>
#include <utils/doublespinboxdelegate.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkplotpiewidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkPlot.h>
#include <vtkChartXY.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkTable.h>
#include <vtkColorSeries.h>
#include <vtkPlotPie.h>
#include <vtkStringArray.h>
#include <vtkChartPie.h>
#include <vtkNew.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>

#include <QDebug>

static const int NUM_ITEMS = 5;
static int sData[] = {77938,9109,2070,12806,19514};
static const char *sLabels[] = {"Books","New and Popular","Periodical","Audiobook","Video"};

PiePlotWidget::PiePlotWidget(QWidget* parent) : PlotWidget(parent)
{
    m_ui = new Ui::TablePlotWidget;
    m_ui->setupUi(this);
    setWindowTitle(tr("Pie Plot"));
    configTableView(m_ui->tableView);

    m_plotWidget = new VtkUtils::VtkPlotPieWidget(this);
    m_ui->verticalLayout->addWidget(m_plotWidget);

    m_ui->tableView->setModel(model());
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);

#if 0
    VTK_CREATE(vtkTable, table);
    VTK_CREATE(vtkIntArray, arrData);
    VTK_CREATE(vtkStringArray, labelArray);

    arrData->SetName("2008 Circulation");
    for (int i = 0; i < NUM_ITEMS; i++) {
        arrData->InsertNextValue(sData[i]);
        labelArray->InsertNextValue(sLabels[i]);
    }

    table->AddColumn(arrData);

    // remove xychart
    vtkChartPie* pieChart = vtkChartPie::SafeDownCast(m_plotWidget->chart());

    // Create a color series to use with our stacks.
    VTK_CREATE(vtkColorSeries, colorSeries);
    colorSeries->SetColorScheme(vtkColorSeries::WARM);

    // Add multiple line plots, setting the colors etc
    vtkPlotPie* pie = vtkPlotPie::SafeDownCast(pieChart->AddPlot(0));
    pie->SetColorSeries(colorSeries);
    pie->SetInputData(table);
    pie->SetInputArray(0,"2008 Circulation");
    pie->SetLabels(labelArray);
    pieChart->SetShowLegend(true);
    pieChart->SetTitle("Circulation 2008");
#endif
}

PiePlotWidget::~PiePlotWidget()
{
    delete m_ui;
}

void PiePlotWidget::onModelRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}
