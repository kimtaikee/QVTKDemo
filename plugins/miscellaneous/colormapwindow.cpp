#include "colormapwindow.h"
#include "ui_colormapwindow.h"

#include <utils/signalblocker.h>
#include <utils/utils.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkConeSource.h>
#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>

#include <QDebug>

ColorMapWindow::ColorMapWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ColorMapWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Color Map"));

    Utils::SignalBlocker sb(m_ui->hueMinSpinBox);
    sb.addObject(m_ui->hueMaxSpinBox);
    sb.addObject(m_ui->saturationMinSpinBox);
    sb.addObject(m_ui->saturationMaxSpinBox);
    sb.addObject(m_ui->valueMinSpinBox);
    sb.addObject(m_ui->valueMaxSpinBox);
    sb.addObject(m_ui->alphaMinSpinBox);
    sb.addObject(m_ui->alphaMaxSpinBox);
    sb.addObject(m_ui->numOfColorsSpinBox);

    m_ui->hueMinSpinBox->setValue(m_hueMin);
    m_ui->hueMaxSpinBox->setValue(m_hueMax);
    m_ui->saturationMinSpinBox->setValue(m_saturationMin);
    m_ui->saturationMaxSpinBox->setValue(m_saturationMax);
    m_ui->valueMinSpinBox->setValue(m_valueMin);
    m_ui->valueMaxSpinBox->setValue(m_valueMax);
    m_ui->alphaMinSpinBox->setValue(m_alphaMin);
    m_ui->alphaMaxSpinBox->setValue(m_alphaMax);
    m_ui->numOfColorsSpinBox->setValue(m_numOfColors);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    apply();

    m_ui->gradientCombo->setCurrentIndex(0);
}

ColorMapWindow::~ColorMapWindow()
{
    delete m_ui;
}

void ColorMapWindow::apply()
{
    VtkUtils::vtkInitOnce(m_modelActor);

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(m_hueMin, m_hueMax);
    lut->SetSaturationRange(m_saturationMin, m_saturationMax);
    lut->SetValueRange(m_valueMin, m_valueMax);
    lut->SetAlphaRange(m_alphaMin, m_alphaMax);
    lut->SetNumberOfColors(m_numOfColors);
    lut->Build();

    VTK_CREATE(vtkConeSource, cone);
    cone->SetResolution(7);
    cone->Update();

    vtkPolyData* polyData = cone->GetOutput();
    int numOfPoints = polyData->GetNumberOfPoints();
    VTK_CREATE(vtkDoubleArray, scalars);
    double pot[3];
    for (int i = 0; i < numOfPoints; ++i) {
        polyData->GetPoint(i, pot);
        scalars->InsertNextTuple1(pot[2]);
    }

    polyData->GetPointData()->SetScalars(scalars);

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(cone->GetOutputPort());
    mapper->SetLookupTable(lut);

    m_modelActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_modelActor);

    VtkUtils::vtkInitOnce(m_scalarBar);
    m_scalarBar->SetNumberOfLabels(m_numOfColors);
    m_scalarBar->SetLookupTable(lut);
    m_scalarBar->SetOrientation(VTK_ORIENT_VERTICAL);
    m_scalarBar->SetHeight(0.7);
    m_scalarBar->SetWidth(0.1);
    m_vtkWidget->defaultRenderer()->AddActor(m_scalarBar);
    m_vtkWidget->showOrientationMarker();
    m_vtkWidget->defaultRenderer()->ResetCamera();
    m_vtkWidget->update();
}

void ColorMapWindow::on_hueMinSpinBox_valueChanged(double arg1)
{
    m_hueMin = arg1;
    apply();
}

void ColorMapWindow::on_hueMaxSpinBox_valueChanged(double arg1)
{
    m_hueMax = arg1;
    apply();
}

void ColorMapWindow::on_saturationMinSpinBox_valueChanged(double arg1)
{
    m_saturationMin = arg1;
    apply();
}

void ColorMapWindow::on_saturationMaxSpinBox_valueChanged(double arg1)
{
    m_saturationMax = arg1;
    apply();
}

void ColorMapWindow::on_valueMinSpinBox_valueChanged(double arg1)
{
    m_valueMin = arg1;
    apply();
}

void ColorMapWindow::on_valueMaxSpinBox_valueChanged(double arg1)
{
    m_valueMax = arg1;
    apply();
}

void ColorMapWindow::on_alphaMinSpinBox_valueChanged(double arg1)
{
    m_alphaMin = arg1;
    apply();
}

void ColorMapWindow::on_alphaMaxSpinBox_valueChanged(double arg1)
{
    m_alphaMax = arg1;
    apply();
}

void ColorMapWindow::on_numOfColorsSpinBox_valueChanged(int arg1)
{
    m_numOfColors = arg1;
    apply();
}

void ColorMapWindow::on_gradientCombo_activated(int index)
{
    Q_UNUSED(index);
    Widgets::GradientComboBox* gcb = qobject_cast<Widgets::GradientComboBox*>(sender());

    double hsv1[3];
    double hsv2[3];
    Utils::qColor2HSV(gcb->currentColor1(), hsv1);
    Utils::qColor2HSV(gcb->currentColor2(), hsv2);

    if (m_scalarBar)
        m_scalarBar->SetTitle(gcb->currentName().toUtf8().data());

    Utils::SignalBlocker sb(m_ui->hueMinSpinBox);
    sb.addObject(m_ui->hueMaxSpinBox);
    sb.addObject(m_ui->saturationMinSpinBox);
    sb.addObject(m_ui->saturationMaxSpinBox);
    sb.addObject(m_ui->valueMinSpinBox);
    sb.addObject(m_ui->valueMaxSpinBox);

    m_ui->hueMinSpinBox->setValue(hsv1[0]);
    m_ui->hueMaxSpinBox->setValue(hsv2[0]);
    m_ui->saturationMinSpinBox->setValue(hsv1[1]);
    m_ui->saturationMaxSpinBox->setValue(hsv2[1]);
    m_ui->valueMinSpinBox->setValue(hsv1[2]);
    m_ui->valueMaxSpinBox->setValue(hsv2[2]);

    m_hueMin = hsv1[0];
    m_hueMax = hsv2[0];
    m_saturationMin = hsv1[1];
    m_saturationMax = hsv2[1];
    m_valueMin = hsv1[2];
    m_valueMax = hsv2[2];

    apply();
}
