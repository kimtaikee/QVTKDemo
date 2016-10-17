#include "piechartactorwindow.h"
#include "ui_plotactorwindow.h"
#include "ui_piesettings.h"

#include <utils/tablemodel.h>
#include <utils/signalblocker.h>

#include <widgets/colorpushbutton.h>

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkPieChartActor.h>
#include <vtkRenderer.h>
#include <vtkFieldData.h>
#include <vtkTextProperty.h>
#include <vtkLegendBoxActor.h>

#include <QDebug>

PieChartActorWindow::PieChartActorWindow(QWidget* parent) : PlotActorWindow(parent)
{
    m_settingsUi = new Ui::PieSettings;
    setupConfigWidget(m_settingsUi);

    setWindowTitle(tr("Pie Plot Actor"));
    initTableModel();

    hookGroupBox(m_settingsUi->titleGroupBox);
    hookGroupBox(m_settingsUi->legendGroupBox);
    hookGroupBox(m_settingsUi->labelGroupBox);

    hookLineEdit(m_settingsUi->titleEdit);

    hookCheckBox(m_settingsUi->titleBoldCheckBox);
    hookCheckBox(m_settingsUi->titleItalicCheckBox);
    hookCheckBox(m_settingsUi->titleShadowCheckBox);
    hookCheckBox(m_settingsUi->showBorderCheckBox);
    hookCheckBox(m_settingsUi->showBoxCheckBox);
    hookCheckBox(m_settingsUi->showScalarCheckBox);
    hookCheckBox(m_settingsUi->useBackgroundCheckBox);

    hookColorButton(m_settingsUi->titleColorButton);
    hookColorButton(m_settingsUi->titleBgColorButton);

    hookSpinBox(m_settingsUi->titleShadowOffset);

    hookDoubleSpinBox(m_settingsUi->titleOpacitySpinBox);

    // left just one column
    Utils::SignalBlocker sb(m_ui->colsSpinBox);
    m_ui->colsSpinBox->setValue(1);
    m_ui->colsSpinBox->setEnabled(false);
}

void PieChartActorWindow::onGroupBoxChecked(bool checked)
{
    QGroupBox* which = qobject_cast<QGroupBox*>(sender());
    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);

    if (which == m_settingsUi->titleGroupBox) {
        piePlotActor->SetTitleVisibility(checked);
    } else if (which == m_settingsUi->legendGroupBox) {
        piePlotActor->SetLegendVisibility(checked);
    } else if (which == m_settingsUi->labelGroupBox) {
        piePlotActor->SetLabelVisibility(checked);
    }

    updateWidget();
}

void PieChartActorWindow::onLineEditTextChanged(const QString &text)
{
    QLineEdit* which = qobject_cast<QLineEdit*>(sender());
    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);

    if (which == m_settingsUi->titleEdit) {
        piePlotActor->SetTitle(text.toUtf8().data());
    }

    updateWidget();
}

void PieChartActorWindow::onCheckBoxStateChanged(int state)
{
    QCheckBox* which = qobject_cast<QCheckBox*>(sender());
    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);
    vtkTextProperty* titleProp = piePlotActor->GetTitleTextProperty();
    bool checked = (state == Qt::Checked);

    if (which == m_settingsUi->titleBoldCheckBox) {
        titleProp->SetBold(checked);
        piePlotActor->SetTitleTextProperty(titleProp);
    } else if (which == m_settingsUi->titleItalicCheckBox) {
        titleProp->SetItalic(checked);
        piePlotActor->SetTitleTextProperty(titleProp);
    } else if (which == m_settingsUi->titleShadowCheckBox) {
        titleProp->SetShadow(checked);
        piePlotActor->SetTitleTextProperty(titleProp);
    } else if (which == m_settingsUi->showBorderCheckBox) {
        vtkLegendBoxActor* legendActor = piePlotActor->GetLegendActor();
        legendActor->SetBorder(checked);
    } else if (which == m_settingsUi->showBoxCheckBox) {
        vtkLegendBoxActor* legendActor = piePlotActor->GetLegendActor();
        legendActor->SetBox(checked);
    } else if (which == m_settingsUi->showScalarCheckBox) {
        vtkLegendBoxActor* legendActor = piePlotActor->GetLegendActor();
        legendActor->SetScalarVisibility(checked);
    } else if (which == m_settingsUi->useBackgroundCheckBox) {
        vtkLegendBoxActor* legendActor = piePlotActor->GetLegendActor();
        legendActor->SetUseBackground(checked);
    }

    updateWidget();
}

void PieChartActorWindow::onColorChanged(const QColor &clr)
{
    Widgets::ColorPushButton* which = qobject_cast<Widgets::ColorPushButton*>(sender());

    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);
    vtkTextProperty* titleProp = piePlotActor->GetTitleTextProperty();
    vtkTextProperty* labelProp = piePlotActor->GetLabelTextProperty();

    if (which == m_settingsUi->titleColorButton) {
        double vtkClr[3];
        Utils::vtkColor(clr, vtkClr);
        titleProp->SetColor(vtkClr);
        piePlotActor->SetTitleTextProperty(titleProp);
    } else if (which == m_settingsUi->titleBgColorButton) {
        double vtkClr[3];
        Utils::vtkColor(clr, vtkClr);
        titleProp->SetBackgroundColor(vtkClr);
        piePlotActor->SetTitleTextProperty(titleProp);
    }

    updateWidget();
}

void PieChartActorWindow::onSpinBoxValueChanged(int value)
{
    QSpinBox* which = qobject_cast<QSpinBox*>(sender());
    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);
    vtkTextProperty* titleProp = piePlotActor->GetTitleTextProperty();

    if (which == m_settingsUi->titleShadowOffset) {
        titleProp->SetShadowOffset(value, value);
        piePlotActor->SetTitleTextProperty(titleProp);
    }

    updateWidget();
}

void PieChartActorWindow::onDoubleSpinBoxValueChanged(double value)
{
    QDoubleSpinBox* which = qobject_cast<QDoubleSpinBox*>(sender());
    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);
    vtkTextProperty* titleProp = piePlotActor->GetTitleTextProperty();

    if (which == m_settingsUi->titleOpacitySpinBox) {
        titleProp->SetOpacity(value);
        piePlotActor->SetTitleTextProperty(titleProp);
    }

    updateWidget();
}

void PieChartActorWindow::createPlotActor()
{
    // prevent multiple creations
    if (m_plotActor)
        return;

    m_plotActor = vtkPieChartActor::New();
    layoutActor(m_plotActor);

    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);
    piePlotActor->SetInputData(createPlotData());
    piePlotActor->SetTitle("Pie Plot");
    piePlotActor->SetDisplayPosition(100, 100);
    piePlotActor->LegendVisibilityOn();
    piePlotActor->SetPickable(true);
    piePlotActor->SetDragable(true);

    m_settingsUi->titleEdit->setText(piePlotActor->GetTitle());

    for (int i = 0; i < DataSize; ++i) {
        vtkColor3ub color = PlotActorWindow::coolColor(i);
        piePlotActor->SetPieceColor(i, color.GetRed(), color.GetGreen(), color.GetBlue());
        piePlotActor->SetPieceLabel(i, tr("Item#%1").arg(i + 1).toUtf8().data());
    }

    m_vtkWidget->defaultRenderer()->AddActor(m_plotActor);
}

void PieChartActorWindow::initTableModel()
{
    m_tableModel->resize(1, m_tableModel->rowCount());
    m_tableModel->random();
}

vtkDataObject* PieChartActorWindow::createPlotData()
{
    vtkFieldData* field = vtkFieldData::New();
    vtkDataObject* data = vtkDataObject::New();

    VTK_CREATE(vtkDoubleArray, values);
    values->SetName("values");

    int rows = m_tableModel->rowCount();
    for (int i = 0; i < rows; ++i)
        values->InsertTuple1(i, m_tableModel->data(i, 0));

    field->AddArray(values);
    data->SetFieldData(field);

    return data;
}

void PieChartActorWindow::render()
{
    vtkPieChartActor* piePlotActor = vtkPieChartActor::SafeDownCast(m_plotActor);
    if (!piePlotActor) {
        qDebug() << "PieChartActorWindow::render: null pie plot actor";
        return;
    }

    // set new data
    piePlotActor->SetInputData(createPlotData());

    int rows = m_tableModel->rowCount();
    for (int i = 0; i < rows; ++i) {
        vtkColor3ub color = PlotActorWindow::coolColor(i);
        piePlotActor->SetPieceColor(i, color.GetRed(), color.GetGreen(), color.GetBlue());
        piePlotActor->SetPieceLabel(i, tr("Item#%1").arg(i + 1).toUtf8().data());
    }

    updateWidget();
}
