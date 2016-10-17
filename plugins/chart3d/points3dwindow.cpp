#include "points3dwindow.h"
#include "ui_chart3dwindow.h"
#include "ui_configpoints3d.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkGlyph3D.h>
#include <vtkSphereSource.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkLabeledDataMapper.h>
#include <vtkProperty2D.h>

#include <QWidget>
#include <QDebug>

Points3DWindow::Points3DWindow(QWidget* parent) : Chart3DWindow(parent)
{
    m_ui->settingsGroupBox->show();
    m_configUi = new Ui::ConfigPoints3D;
    setupConfigWidget(m_configUi);

    m_configUi->labelsFormatEdit->setText(m_labelsFormat);
    m_configUi->scaleModeCombo->setCurrentIndex(m_scaleMode);
    m_configUi->scaleFactorSpinBox->setValue(m_scaleFactor);
    m_configUi->scalingGroupBox->setChecked(m_scaling);
    m_configUi->thetaResolutionSpinBox->setValue(m_thetaResolution);
    m_configUi->phiResolutionSpinBox->setValue(m_phiResolution);
    m_configUi->labelsColorButton->setCurrentColor(m_labelsColor);

    connect(m_configUi->scaleModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_scaleModeCombo_currentIndexChanged(int)));
    connect(m_configUi->scaleFactorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_scaleFactorSpinBox_valueChanged(double)));
    connect(m_configUi->scalingGroupBox, SIGNAL(toggled(bool)), this, SLOT(on_scalingGroupBox_toggled(bool)));
    connect(m_configUi->thetaResolutionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_thetaResolutionSpinBox_valueChanged(int)));
    connect(m_configUi->phiResolutionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_phiResolutionSpinBox_valueChanged(int)));
    connect(m_configUi->labelsColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_configUi->labelsGroupBox, SIGNAL(toggled(bool)), this, SLOT(on_labelsGroupBox_toggled(bool)));
    connect(m_configUi->labelsFormatEdit, SIGNAL(textChanged(QString)), this, SLOT(on_labelsFormatEdit_textChanged(QString)));

    setWindowTitle(tr("Points3D"));
}

Points3DWindow::~Points3DWindow()
{
    delete m_configUi;
}

void Points3DWindow::createChart(vtkPolyData* polydata)
{
    if (!polydata)
        return;

    Chart3DWindow::createChart(polydata);

    showOrientationMarker();

    VtkUtils::vtkInitOnce(m_cubeAxes);
    m_cubeAxes->SetFontFactor(1.0);
    m_cubeAxes->SetFlyModeToNone();
    m_cubeAxes->SetShowActualBounds(1);
    m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());
    m_cubeAxes->SetInputData(polydata);
    m_vtkWidget->defaultRenderer()->AddViewProp(m_cubeAxes);

    updateChart();

    QColor bgClr(0, 170, 255);
    double bgClrArr[3];
    Utils::vtkColor(bgClr, bgClrArr);
    m_vtkWidget->defaultRenderer()->SetBackground(bgClrArr); // set background color
    m_vtkWidget->defaultRenderer()->AddActor(m_chartActor);
    m_vtkWidget->defaultRenderer()->ResetCamera();
    m_vtkWidget->showOrientationMarker();
    m_vtkWidget->update();
}

void Points3DWindow::updateChart()
{
    qDebug() << __FUNCTION__;

    double minScale = qMin(xRange(), qMin(yRange(), zRange()));
    double radius = minScale / 100;

    VtkUtils::vtkInitOnce(m_chartActor);
//    VtkUtils::vtkInitOnce(&m_cubeAxes);
//    m_cubeAxes->SetFontFactor(1.0);
//    m_cubeAxes->SetFlyModeToNone();
//    m_cubeAxes->SetShowActualBounds(1);
//    m_cubeAxes->SetCamera(m_vtkWidget->defaultRenderer()->GetActiveCamera());
//    m_cubeAxes->SetInputData(polyData());
//    m_vtkWidget->defaultRenderer()->AddViewProp(m_cubeAxes);

    VTK_CREATE(vtkSphereSource, sphereSrc);
    sphereSrc->SetThetaResolution(m_thetaResolution);
    sphereSrc->SetPhiResolution(m_phiResolution);
    sphereSrc->SetRadius(radius);
    sphereSrc->Update();

    VTK_CREATE(vtkGlyph3D, glyph3d);
    glyph3d->SetInputData(polyData());

    glyph3d->SetScaling(m_scaling);
    glyph3d->SetScaleMode(m_scaleMode);
    glyph3d->SetScaleFactor(m_scaleFactor);
    glyph3d->SetSourceConnection(sphereSrc->GetOutputPort());
    glyph3d->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputData(glyph3d->GetOutput());
    mapper->ScalarVisibilityOff(); // default to scalar off

    if (m_useScalarColor) {
        VTK_CREATE(vtkLookupTable, lut);
        lut->SetRange(m_bounds[4], m_bounds[5]); // z range
        lut->SetHueRange(0.6667, 0.0);
        lut->Build();
        mapper->SetLookupTable(lut);
        mapper->ScalarVisibilityOn();
    }

    m_chartActor->SetMapper(mapper);

    double clr[3];
    Utils::vtkColor(m_customColor, clr);
    m_chartActor->GetProperty()->SetColor(clr);
    m_vtkWidget->update();

    if (m_labelsActor.Get() && m_labelsActor->GetVisibility())
        showLabels();
}

void Points3DWindow::createUi()
{

}

void Points3DWindow::on_thetaResolutionSpinBox_valueChanged(int arg1)
{
    m_thetaResolution = arg1;
    updateChart();
}

void Points3DWindow::on_phiResolutionSpinBox_valueChanged(int arg1)
{
    m_phiResolution = arg1;
    updateChart();
}

void Points3DWindow::on_scaleModeCombo_currentIndexChanged(int index)
{
    m_scaleMode = index;
    updateChart();
}

void Points3DWindow::on_scaleFactorSpinBox_valueChanged(double arg1)
{
    m_scaleFactor = arg1;
    updateChart();
}

void Points3DWindow::on_scalingGroupBox_toggled(bool arg1)
{
    m_scaling = arg1;
    updateChart();
}

void Points3DWindow::on_labelsGroupBox_toggled(bool arg1)
{
    showLabels(arg1);
}

void Points3DWindow::onColorChanged(const QColor& clr)
{
    Widgets::ColorPushButton* button = qobject_cast<Widgets::ColorPushButton*>(sender());

    if (button == m_configUi->labelsColorButton) {
        m_labelsColor = clr;
        if (m_labelsActor.Get()) {
            double clrArr[3];
            Utils::vtkColor(clr, clrArr);
            m_labelsActor->GetProperty()->SetColor(clrArr);
            m_vtkWidget->update();
        }
    }
}

void Points3DWindow::showLabels(bool show)
{
    if (!show && !m_labelsActor.Get())
        return;

    if (m_labelsActor.Get() && !show) {
        m_labelsActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    if (!m_labelsActor.Get())
        m_labelsActor = vtkActor2D::New();

    VTK_CREATE(vtkLabeledDataMapper, labelMapper);
    labelMapper->SetFieldDataName("Isovalues");
    labelMapper->SetInputData(polyData());
    labelMapper->SetLabelMode(1);
    labelMapper->SetLabelFormat(m_labelsFormat.toUtf8().data());

    double clrArr[3];
    Utils::vtkColor(m_labelsColor, clrArr);

    m_labelsActor->SetMapper(labelMapper);
    m_labelsActor->SetVisibility(show);
    m_labelsActor->GetProperty()->SetColor(clrArr);
    m_vtkWidget->defaultRenderer()->AddActor(m_labelsActor);
    m_vtkWidget->update();
}

void Points3DWindow::on_labelsFormatEdit_textChanged(const QString &arg1)
{
    m_labelsFormat = arg1;
    if (m_labelsActor.Get() && m_labelsActor->GetVisibility())
        showLabels();
}
