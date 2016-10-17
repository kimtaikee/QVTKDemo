#include "spiderplotactorwindow.h"
#include "ui_plotactorwindow.h"
#include "ui_spidersettings.h"

#include <utils/utils.h>
#include <utils/tablemodel.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkSpiderPlotActor.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkFieldData.h>
#include <vtkDataObject.h>
#include <vtkLegendBoxActor.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkColorSeries.h>

SpiderPlotActorWindow::SpiderPlotActorWindow(QWidget* parent) : PlotActorWindow(parent)
{
    m_settingsUi = new Ui::SpiderSettings;
    setupConfigWidget(m_settingsUi);

    // initialize settings
    m_settingsUi->titleEdit->setText(tr("Spider Plot Actor"));
    m_settingsUi->titleGroupBox->setChecked(true);
    m_settingsUi->showLabelsCheckBox->setChecked(true);
    m_settingsUi->showLegendCheckBox->setChecked(true);

    connect(m_settingsUi->titleGroupBox, SIGNAL(toggled(bool)), this, SLOT(on_titleGroupBox_toggled(bool)));
    connect(m_settingsUi->titleEdit, SIGNAL(textChanged(QString)), this, SLOT(on_titleEdit_textChanged(QString)));
    connect(m_settingsUi->showLegendCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_showLegendCheckBox_toggled(bool)));
    connect(m_settingsUi->showLabelsCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_showLabelsCheckBox_toggled(bool)));

    setWindowTitle(tr("Spider Plot Actor"));
    initTableModel();

    Utils::SignalBlocker sb(m_ui->colsSpinBox);
    m_ui->colsSpinBox->setValue(m_tableModel->columnCount());
}

void SpiderPlotActorWindow::createPlotActor()
{
    // prevent multiple creations
    if (m_plotActor)
        return;

    m_plotActor = vtkSpiderPlotActor::New();
    layoutActor(m_plotActor);

    vtkSpiderPlotActor* spiderPlot = vtkSpiderPlotActor::SafeDownCast(m_plotActor);
//    spiderPlot->LegendVisibilityOn();
    spiderPlot->SetTitle("spider plot");
    spiderPlot->SetIndependentVariablesToColumns();
    spiderPlot->GetProperty()->SetColor(1, 0, 0);

    m_vtkWidget->defaultRenderer()->AddActor(m_plotActor);
}

vtkDataObject* SpiderPlotActorWindow::createPlotData()
{
    int rows = m_tableModel->rowCount();
    int cols = m_tableModel->columnCount();

    vtkDataObject* dataObj = vtkDataObject::New();

    QString colName;
    for (int col = 0; col < cols; ++col) {
        colName = QString("column#%1").arg(col + 1);

        VTK_CREATE(vtkFloatArray, arr);
        arr->SetNumberOfTuples(rows);
        for (int row = 0; row < rows; ++row)
            arr->SetTuple1(row, m_tableModel->data(row, col));
        dataObj->GetFieldData()->AddArray(arr);
    }

    return dataObj;
}

void SpiderPlotActorWindow::render()
{
    vtkSpiderPlotActor* spiderPlot = vtkSpiderPlotActor::SafeDownCast(m_plotActor);
    spiderPlot->SetInputData(createPlotData());

    int cols = m_tableModel->columnCount();
    int rows = m_tableModel->rowCount();

    spiderPlot->GetLegendActor()->SetNumberOfEntries(rows);

    for (int i = 0; i < cols; ++i) {
        vtkColor3ub color = PlotActorWindow::coolColor(i);

        spiderPlot->SetAxisLabel(i, QString("column#%1").arg(i + 1).toUtf8().data());
        spiderPlot->SetAxisRange(i, m_ui->minSpinBox->value(), m_ui->maxSpinBox->value());
        spiderPlot->SetPlotColor(i, color.GetRed(), color.GetGreen(), color.GetBlue());
    }

    updateWidget();
}

void SpiderPlotActorWindow::initTableModel()
{
    static const int ColumnCount = 4;
    QVariantList labels;
    for (int i = 0; i < ColumnCount; ++i)
        labels << "Y";

    m_tableModel->setHorizontalHeaderData(labels);
    m_tableModel->resize(ColumnCount, m_tableModel->rowCount());
    m_tableModel->random();
}

void SpiderPlotActorWindow::on_titleGroupBox_toggled(bool arg1)
{
    vtkSpiderPlotActor* spiderPlot = vtkSpiderPlotActor::SafeDownCast(m_plotActor);
    spiderPlot->SetTitleVisibility(arg1);
    updateWidget();
}

void SpiderPlotActorWindow::on_titleEdit_textChanged(const QString &arg1)
{
    vtkSpiderPlotActor* spiderPlot = vtkSpiderPlotActor::SafeDownCast(m_plotActor);
    spiderPlot->SetTitle(arg1.toUtf8().data());
    updateWidget();
}

void SpiderPlotActorWindow::on_showLabelsCheckBox_toggled(bool checked)
{
    vtkSpiderPlotActor* spiderPlot = vtkSpiderPlotActor::SafeDownCast(m_plotActor);
    spiderPlot->SetLabelVisibility(checked);
    updateWidget();
}

void SpiderPlotActorWindow::on_showLegendCheckBox_toggled(bool checked)
{
    vtkSpiderPlotActor* spiderPlot = vtkSpiderPlotActor::SafeDownCast(m_plotActor);
    spiderPlot->SetLegendVisibility(checked);
    updateWidget();
}
