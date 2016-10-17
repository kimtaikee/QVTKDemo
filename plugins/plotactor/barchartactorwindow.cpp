#include "barchartactorwindow.h"
#include "ui_spidersettings.h"
#include "ui_plotactorwindow.h"

#include <utils/tablemodel.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkBarChartActor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkIntArray.h>
#include <vtkJPEGReader.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAccumulate.h>
#include <vtkDataObject.h>
#include <vtkFieldData.h>
#include <vtkImageData.h>
#include <vtkDoubleArray.h>

BarChartActorWindow::BarChartActorWindow(QWidget* parent) : PlotActorWindow(parent)
{
    m_settingsUi = new Ui::SpiderSettings;
    setupConfigWidget(m_settingsUi);

    // initialize settings
    m_settingsUi->titleEdit->setText(tr("Bar Plot Actor"));
    m_settingsUi->titleGroupBox->setChecked(true);
    m_settingsUi->showLabelsCheckBox->setChecked(true);
    m_settingsUi->showLegendCheckBox->setChecked(true);

    connect(m_settingsUi->titleGroupBox, SIGNAL(toggled(bool)), this, SLOT(on_titleGroupBox_toggled(bool)));
    connect(m_settingsUi->titleEdit, SIGNAL(textChanged(QString)), this, SLOT(on_titleEdit_textChanged(QString)));
    connect(m_settingsUi->showLegendCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_showLegendCheckBox_toggled(bool)));
    connect(m_settingsUi->showLabelsCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_showLabelsCheckBox_toggled(bool)));

    setWindowTitle(tr("Bar Plot Actor"));
}

void BarChartActorWindow::createPlotActor()
{
    // prevent multiple creations
    if (m_plotActor)
        return;

    m_plotActor = vtkBarChartActor::New();
    layoutActor(m_plotActor);

    vtkBarChartActor* barPlotActor = vtkBarChartActor::SafeDownCast(m_plotActor);
    barPlotActor->SetTitle("Bar Plot");
    barPlotActor->SetTitleVisibility(1);
    barPlotActor->SetLabelVisibility(1);
    barPlotActor->LegendVisibilityOff();
    barPlotActor->SetYTitle("Frequency");

    m_vtkWidget->defaultRenderer()->AddActor(m_plotActor);
}

void BarChartActorWindow::render()
{
    vtkBarChartActor* barPlotActor = vtkBarChartActor::SafeDownCast(m_plotActor);

    ///
    int cols = m_tableModel->columnCount();
    int rows = m_tableModel->rowCount();

    VTK_CREATE(vtkDataObject, data);
    VTK_CREATE(vtkDoubleArray, values);

    // a value map to a bar
    double clr[3];
    int counter = 0;
    for (int col = 0; col < cols; ++col) {
        for (int row = 0; row < rows; ++row) {
            values->InsertTuple1(counter, m_tableModel->data(row, col));
            vtkColor3ub color = PlotActorWindow::coolColor(col);
            Utils::vtkColor(QColor(color.GetRed(), color.GetGreen(), color.GetBlue()), clr);
            barPlotActor->SetBarColor(counter, clr);
            barPlotActor->SetBarLabel(counter, tr("Bar#%1").arg(counter + 1).toUtf8().data());
            ++counter;
        }
    }

    data->GetFieldData()->AddArray(values);
    barPlotActor->SetInput(data);

    updateWidget();
}

void BarChartActorWindow::layoutActor(vtkActor2D *actor)
{
    actor->GetPositionCoordinate()->SetValue(0.1, 0.1);
    actor->GetPosition2Coordinate()->SetValue(1.0, 1.0);
}

void BarChartActorWindow::on_titleGroupBox_toggled(bool arg1)
{
    vtkBarChartActor* barChart = vtkBarChartActor::SafeDownCast(m_plotActor);
    barChart ->SetTitleVisibility(arg1);
    updateWidget();
}

void BarChartActorWindow::on_titleEdit_textChanged(const QString &arg1)
{
    vtkBarChartActor* barChart = vtkBarChartActor::SafeDownCast(m_plotActor);
    barChart->SetTitle(arg1.toUtf8().data());
    updateWidget();
}

void BarChartActorWindow::on_showLabelsCheckBox_toggled(bool checked)
{
    vtkBarChartActor* barChart = vtkBarChartActor::SafeDownCast(m_plotActor);
    barChart->SetLabelVisibility(checked);
    updateWidget();
}

void BarChartActorWindow::on_showLegendCheckBox_toggled(bool checked)
{
    vtkBarChartActor* barChart = vtkBarChartActor::SafeDownCast(m_plotActor);
    barChart->SetLegendVisibility(checked);
    updateWidget();
}
