#include "contourwidgetwindow.h"
#include "ui_contourwidgetconfig.h"
#include "ui_basewidgetwindow.h"

#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/contourwidgetobserver.h>

#include <vtkContourWidget.h>
#include <vtkContourRepresentation.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkRenderer.h>
#include <vtkContextActor.h>
#include <vtkContextScene.h>
#include <vtkPlotPie.h>
#include <vtkTable.h>
#include <vtkChartPie.h>
#include <vtkIntArray.h>
#include <vtkColorSeries.h>
#include <vtkStringArray.h>
#include <vtkChartXY.h>
#include <vtkFloatArray.h>
#include <vtkRenderWindow.h>

#include <QDebug>

static const int NUM_ITEMS = 5;
static int sData[] = {77938,9109,2070,12806,19514};
static const char *sLabels[] = {"Books","New and Popular","Periodical","Audiobook","Video"};

ContourWidgetWindow::ContourWidgetWindow(QWidget* parent) : BaseWidgetWindow(parent)
{
    createUi();
    createWidget();
}

void ContourWidgetWindow::createWidget()
{
    VtkUtils::vtkInitOnce(m_contour);
    m_contour->CreateDefaultRepresentation();
    m_contour->SetInteractor(m_vtkWidget->GetInteractor());
    m_contour->On();
    m_vtkWidget->defaultRenderer()->ResetCamera();

    VtkUtils::ContourWidgetObserver* observer = new VtkUtils::ContourWidgetObserver(this);
    observer->attach(m_contour);
    connect(observer, SIGNAL(dataChanged(vtkPolyData*)), this, SLOT(onDataChanged(vtkPolyData*)));
}

void ContourWidgetWindow::createUi()
{
    m_configUi = new Ui::ContourWidgetConfig;
    setupConfigWidget(m_configUi);
    m_configUi->clearButton->hide();
    m_configUi->showNodesCheckBox->hide();
    BaseWidgetWindow::createUi();
    setWindowTitle(tr("Contour Widget"));

#if 0
    VTK_CREATE(vtkChartPie, chart);
    scene->AddItem(chart);
    chart->SetAutoSize(false);
    chart->SetSize(vtkRectf(0, 0, 300, 200));

    chart->SetAutoSize(false);
    chart->SetSize(vtkRectf(0.0, 0.0, 300, 200));

//    chart->SetGeometry(100, 100);

    // Create a table with some points in it...
    VTK_CREATE(vtkTable, table);
    VTK_CREATE(vtkIntArray, arrData);
    VTK_CREATE(vtkStringArray, labelArray);

    arrData->SetName("2008 Circulation");
    for (int i = 0; i < NUM_ITEMS; i++) {
        arrData->InsertNextValue(sData[i]);
        labelArray->InsertNextValue(sLabels[i]);
    }

    table->AddColumn(arrData);

    // Create a color series to use with our stacks.
    VTK_CREATE(vtkColorSeries, colorSeries);
    colorSeries->SetColorScheme(vtkColorSeries::WARM);

    // Add multiple line plots, setting the colors etc
    vtkPlotPie *pie = vtkPlotPie::SafeDownCast(chart->AddPlot(0));
    pie->SetColorSeries(colorSeries);
    pie->SetInputData(table);
    pie->SetInputArray(0,"2008 Circulation");
    pie->SetLabels(labelArray);

    chart->SetShowLegend(true);
    chart->SetTitle("Circulation 2008");
#endif

//#define PLOT_LINE
#ifdef PLOT_LINE
    VTK_CREATE(vtkTable, table);

    VTK_CREATE(vtkFloatArray, arrX);
    arrX->SetName("X Axis");
    table->AddColumn(arrX);

    VTK_CREATE(vtkFloatArray, arrC);
    arrC->SetName("Cosine");
    table->AddColumn(arrC);

    VTK_CREATE(vtkFloatArray, arrS);
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

    VTK_CREATE(vtkChartXY, chart);
    scene->AddItem(chart);
    chart->SetAutoSize(false);
    chart->SetSize(vtkRectf(10, 10, 300, 200));

    vtkPlot *line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(1.0);

    line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 2);
    line->SetColor(255, 0, 0, 255);
    line->SetWidth(5.0);
#endif

    m_vtkWidget->GetRenderWindow()->SetMultiSamples(0);
}

void ContourWidgetWindow::on_clearButton_clicked()
{
    vtkContourRepresentation* rep = vtkContourRepresentation::SafeDownCast(m_contour->GetRepresentation());
    rep->ClearAllNodes();
    m_vtkWidget->update();
}

void ContourWidgetWindow::on_showNodesCheckBox_toggled(bool checked)
{
    vtkContourRepresentation* rep = vtkContourRepresentation::SafeDownCast(m_contour->GetRepresentation());
    rep->SetShowSelectedNodes(checked);
    m_vtkWidget->update();
}

void ContourWidgetWindow::onDataChanged(vtkPolyData* pd)
{
    if (!pd)
        return;

    int numOfPoints = pd->GetNumberOfPoints();
    qDebug() << "num of points: " << numOfPoints;
}

