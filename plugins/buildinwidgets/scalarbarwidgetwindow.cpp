#include "scalarbarwidgetwindow.h"
#include "ui_scalarbarwidgetconfig.h"
#include "ui_basewidgetwindow.h"

#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkScalarBarWidget.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>

ScalarBarWidgetWindow::ScalarBarWidgetWindow(QWidget* parent) : BaseWidgetWindow(parent)
{
    createUi();
    createWidget();
}

void ScalarBarWidgetWindow::createWidget()
{
    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(0.6667, 0.0);
    lut->SetNumberOfColors(10);
    lut->Build();

    VTK_CREATE(vtkScalarBarActor, sbActor);
    sbActor->SetOrientationToHorizontal();
    sbActor->SetNumberOfLabels(10);
    sbActor->SetLookupTable(lut);

    VtkUtils::vtkInitOnce(m_rep);
    VtkUtils::vtkInitOnce(m_scalarBar);
    m_scalarBar->SetRepresentation(m_rep);
    m_scalarBar->SetInteractor(m_vtkWidget->GetInteractor());
    m_scalarBar->SetScalarBarActor(sbActor);
    m_scalarBar->On();
    m_vtkWidget->defaultRenderer()->ResetCamera();

    // config default settings
    m_configUi->gradientCombo->setCurrentIndex(1);
}

void ScalarBarWidgetWindow::createUi()
{
    m_configUi = new Ui::ScalarBarWidgetConfig;
    setupConfigWidget(m_configUi);

    Utils::SignalBlocker sb(m_configUi->orientCombo);
    sb.addObject(m_configUi->numOfLabelsSpinBox);
    sb.addObject(m_configUi->numOfColorsSpinBox);

    m_configUi->orientCombo->setCurrentIndex(1);
    m_configUi->numOfLabelsSpinBox->setValue(10);
    m_configUi->numOfColorsSpinBox->setValue(10);

    BaseWidgetWindow::createUi();

    setWindowTitle(tr("ScalarBar Widget"));
}

void ScalarBarWidgetWindow::on_orientCombo_currentIndexChanged(int index)
{
    m_rep->SetOrientation(index);
    m_vtkWidget->update();
}

void ScalarBarWidgetWindow::on_gradientCombo_activated(int index)
{
    Q_UNUSED(index)
    Widgets::GradientComboBox* gcb = qobject_cast<Widgets::GradientComboBox*>(sender());
    QColor color1 = gcb->currentColor1();
    QColor color2 = gcb->currentColor2();
    QString name = gcb->currentName();

    double hsv1[3];
    double hsv2[3];

    Utils::vtkColor(color1, hsv1);
    Utils::vtkColor(color2, hsv2);

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(hsv1[0], hsv2[0]);
    lut->SetSaturationRange(hsv1[1], hsv2[1]);
    lut->SetValueRange(hsv1[2], hsv2[2]);
    lut->SetTableRange(0.0, 1.0);
    lut->SetNumberOfColors(10);
    lut->Build();

    m_scalarBar->GetScalarBarActor()->SetLookupTable(lut);
    m_scalarBar->GetScalarBarActor()->SetTitle(name.toUtf8().data());
    m_vtkWidget->update();
}

void ScalarBarWidgetWindow::on_numOfLabelsSpinBox_valueChanged(int arg1)
{
    m_scalarBar->GetScalarBarActor()->SetNumberOfLabels(arg1);
    m_vtkWidget->update();
}

void ScalarBarWidgetWindow::on_numOfColorsSpinBox_valueChanged(int arg1)
{
    QPair<QColor, QColor> clrPair = m_configUi->gradientCombo->colorPair(m_configUi->gradientCombo->currentIndex());
    QColor color1 = clrPair.first;
    QColor color2 = clrPair.second;

    double hsv1[3];
    double hsv2[3];

    Utils::vtkColor(color1, hsv1);
    Utils::vtkColor(color2, hsv2);

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(hsv1[0], hsv2[0]);
    lut->SetSaturationRange(hsv1[1], hsv2[1]);
    lut->SetValueRange(hsv1[2], hsv2[2]);
    lut->SetTableRange(0.0, 1.0);
    lut->SetNumberOfColors(arg1);
    lut->Build();

    m_scalarBar->GetScalarBarActor()->SetLookupTable(lut);
    m_vtkWidget->update();
}

void ScalarBarWidgetWindow::on_selectableCheckBox_toggled(bool checked)
{
    m_scalarBar->SetSelectable(checked);
}

void ScalarBarWidgetWindow::on_resizableCheckBox_toggled(bool checked)
{
    m_scalarBar->SetResizable(checked);
}
