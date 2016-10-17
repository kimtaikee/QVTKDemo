#include "smoothwindow.h"
#include "ui_smoothconfig.h"
#include "ui_generalfilterwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkDecimatePro.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmoothPolyDataFilter.h>

SmoothWindow::SmoothWindow(QWidget* parent) : FilterWindow(parent)
{
    createUi();
    setWindowTitle(tr("Smooth"));
}

void SmoothWindow::apply()
{
    if (!m_dataObject) {
        qDebug() << "SmoothWindow::apply: null data object.";
        return;
    }

    VTK_CREATE(vtkSmoothPolyDataFilter, smooth);
    smooth->SetInputData(m_dataObject);
    smooth->SetNumberOfIterations(m_numberOfIterations);
    smooth->SetFeatureEdgeSmoothing(m_featureEdgeSmoothing);
    smooth->SetBoundarySmoothing(m_boundarySmoothing);
    smooth->SetConvergence(m_convergence);
    smooth->SetFeatureAngle(m_featureAngle);
    smooth->SetEdgeAngle(m_edgeAngle);
    smooth->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(smooth->GetOutputPort());

    VtkUtils::vtkInitOnce(&m_modelActor);
    m_modelActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_modelActor);
    update();
}

void SmoothWindow::on_openButton_clicked()
{
    browseFile();
    m_configUi->fileEdit->setText(fileName());
}

void SmoothWindow::on_convergenceSpinBox_valueChanged(double arg1)
{
    m_convergence = arg1;
    apply();
}

void SmoothWindow::on_numOfIterationsSpinBox_valueChanged(int arg1)
{
    m_numberOfIterations = arg1;
    apply();
}

void SmoothWindow::on_featureEdgeSmoothingCheckBox_toggled(bool checked)
{
    m_featureEdgeSmoothing = checked;
    apply();
}

void SmoothWindow::on_boundarySmoothingCheckBox_toggled(bool checked)
{
    m_boundarySmoothing = checked;
    apply();
}

void SmoothWindow::on_featureAngleSpinBox_valueChanged(double arg1)
{
    m_featureAngle = arg1;
    apply();
}

void SmoothWindow::on_edgeAngleSpinBox_valueChanged(double arg1)
{
    m_edgeAngle = arg1;
    apply();
}

void SmoothWindow::createUi()
{
    m_configUi = new Ui::SmoothConfig;
    setupConfigWidget(m_configUi);

    FilterWindow::createUi();
}
