#include "glyphwindow.h"
#include "ui_glyphconfig.h"
#include "ui_generalfilterwindow.h"

#include <utils/utils.h>
#include <utils/tablemodel.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>
#include <vtkConeSource.h>
#include <vtkLineSource.h>
#include <vtkCylinderSource.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkLODActor.h>
#include <vtkLabeledDataMapper.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPlaneSource.h>

#include <QWidget>

GlyphWindow::GlyphWindow(QWidget* parent) : FilterWindow(parent)
{
    createUi();
    setObjectName("GlyphWindow");
    setWindowTitle(tr("Glyph"));

    Utils::SignalBlocker sb(m_configUi->sizeSpinBox);
    sb.addObject(m_configUi->glyphColorButton);
    sb.addObject(m_configUi->labelColorButton);
    sb.addObject(m_configUi->rowsSpinBox);
    sb.addObject(m_configUi->modeCombo);

    m_configUi->sizeSpinBox->setValue(m_size);
    m_configUi->glyphColorButton->setCurrentColor(m_glyphColor);
    m_configUi->labelColorButton->setCurrentColor(m_labelColor);
    m_configUi->rowsSpinBox->setValue(DefaultRows);
    m_configUi->modeCombo->setCurrentIndex(m_labelMode);

    initTableModel();

    m_configUi->dataTable->setModel(m_tableModel);
    connect(m_configUi->glyphColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));
    connect(m_configUi->labelColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));

    //
    fireupModelToPointsConverter();
}

void GlyphWindow::apply()
{
    if (!m_dataObject)
        return;

    VTK_CREATE(vtkGlyph3D, glyph3d);
    glyph3d->SetInputData(m_dataObject);
    glyph3d->SetScaleModeToDataScalingOff();
    glyph3d->SetScaling(0);

    switch (m_shape) {
    case Arrow:
    {
        VTK_CREATE(vtkArrowSource, as);
        glyph3d->SetSourceConnection(as->GetOutputPort());
    }
        break;

    case Cone:
    {
        VTK_CREATE(vtkConeSource, cs);
        glyph3d->SetSourceConnection(cs->GetOutputPort());
    }
        break;

    case Line:
    {
        VTK_CREATE(vtkLineSource, ls);
        glyph3d->SetSourceConnection(ls->GetOutputPort());
    }
        break;

    case Cylinder:
    {
        VTK_CREATE(vtkCylinderSource, cs);
        glyph3d->SetSourceConnection(cs->GetOutputPort());
    }
        break;

    case Sphere:
    {
        VTK_CREATE(vtkSphereSource, ss);
        glyph3d->SetSourceConnection(ss->GetOutputPort());
    }
        break;

    case Point:
    {
        VTK_CREATE(vtkPointSource, ps);
        ps->SetNumberOfPoints(1);
        glyph3d->SetSourceConnection(ps->GetOutputPort());
    }
     break;
    }

    // Create a mapper and actor
    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(glyph3d->GetOutputPort());
    mapper->SetScalarVisibility(!m_glyphColor.isValid());

    VtkUtils::vtkInitOnce(&m_filterActor);
    m_filterActor->GetProperty()->SetPointSize(m_size);

    double vtkClr[3];
    Utils::vtkColor(m_glyphColor, vtkClr);
    m_filterActor->GetProperty()->SetColor(vtkClr);

    m_filterActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_filterActor);
    m_vtkWidget->defaultRenderer()->ResetCamera();

    showLabels(m_labelVisible);

    applyDisplayEffect();
}

void GlyphWindow::modelReady()
{
    FilterWindow::modelReady();
    apply();
}

void GlyphWindow::createUi()
{
    m_configUi = new Ui::GlyphConfig;
    setupConfigWidget(m_configUi);

    FilterWindow::createUi();
}

void GlyphWindow::on_sizeSpinBox_valueChanged(double arg1)
{
    m_size = arg1;
    apply();
}

void GlyphWindow::on_shapeCombo_currentIndexChanged(int index)
{
    m_shape = static_cast<Shape>(index);
    apply();
}

void GlyphWindow::onColorChanged(const QColor& clr)
{
    Widgets::ColorPushButton* button = qobject_cast<Widgets::ColorPushButton*>(sender());

    if (button == m_configUi->glyphColorButton)
        m_glyphColor = clr;
    else if (button == m_configUi->labelColorButton) {
        m_labelColor = clr;
        setLabelsColor(clr);
    }

    apply();
}

void GlyphWindow::on_openButton_clicked()
{
    browseFile();
    m_configUi->fileEdit->setText(m_fileName);
}

void GlyphWindow::on_displayEffectCombo_currentIndexChanged(int index)
{
    setDisplayEffect(static_cast<DisplayEffect>(index));
}

void GlyphWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    m_tableModel->resize(m_tableModel->columnCount(), arg1);
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void GlyphWindow::on_labelGroupBox_toggled(bool arg1)
{
    m_labelVisible = arg1;
    showLabels(arg1);
}

void GlyphWindow::showLabels(bool show)
{
    if (!show && !m_labelActor)
        return;

    if (m_labelActor && !show) {
        m_labelActor->SetVisibility(show);
        update();
        return;
    }

    VtkUtils::vtkInitOnce(&m_labelActor);

    VTK_CREATE(vtkLabeledDataMapper, labelMapper);
    labelMapper->SetFieldDataName("Isovalues");
    labelMapper->SetInputData(m_dataObject);
    labelMapper->SetLabelMode(m_labelMode);
    labelMapper->SetLabelFormat("%6.2f");

    m_labelActor->SetMapper(labelMapper);
    m_labelActor->SetVisibility(show);
    m_vtkWidget->defaultRenderer()->AddActor(m_labelActor);
    update();
}

void GlyphWindow::setLabelsColor(const QColor& clr)
{
    if (!m_labelActor)
        return;

    double vtkClr[3];
    Utils::vtkColor(clr, vtkClr);
    m_labelActor->GetProperty()->SetColor(vtkClr);
    update();
}

void GlyphWindow::on_modeCombo_currentIndexChanged(int index)
{
    m_labelMode = index;
    showLabels();
}

void GlyphWindow::on_randomButton_clicked()
{
    randomTableModel();
}
