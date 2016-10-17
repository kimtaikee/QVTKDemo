#include "decimatewindow.h"
#include "ui_decimateconfig.h"
#include "ui_generalfilterwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkDecimatePro.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <QDebug>

DecimateWindow::DecimateWindow(QWidget* parent) : FilterWindow(parent)
{
    createUi();
    setWindowTitle(tr("Decimate"));
}

void DecimateWindow::apply()
{
    if (!m_dataObject) {
        qDebug() << "DecimateWindow::apply: null data object.";
        return;
    }

    qDebug() << __FUNCTION__;

    VTK_CREATE(vtkDecimatePro, decimate);
    decimate->SetTargetReduction(m_targetReduction);
    decimate->SetFeatureAngle(m_featureAngle);
    decimate->SetSplitAngle(m_splitAngle);
    decimate->SetInflectionPointRatio(m_inflectionPointRatio);
    decimate->SetPreserveTopology(m_preserveTopology);
    decimate->SetSplitting(m_splitting);
    decimate->SetPreSplitMesh(m_presplitMesh);
    decimate->SetAccumulateError(m_accumulateError);
    decimate->SetOutputPointsPrecision(m_outputPointsPrecision);
    decimate->SetDegree(m_degree);
    decimate->SetBoundaryVertexDeletion(m_boundaryVertexDeletion);
    decimate->SetInputData(m_dataObject);

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(decimate->GetOutputPort());

    VtkUtils::vtkInitOnce(&m_modelActor);
    m_modelActor->SetMapper(mapper);

    m_vtkWidget->defaultRenderer()->AddActor(m_modelActor);
    update();
}

void DecimateWindow::on_targetReductionSpinBox_valueChanged(double arg1)
{
    m_targetReduction = arg1;
    apply();
}

void DecimateWindow::on_preserveTopologyCheckBox_toggled(bool checked)
{
    m_preserveTopology = checked;
    apply();
}

void DecimateWindow::on_splittingCheckBox_toggled(bool checked)
{
    m_splitting = checked;
    apply();
}

void DecimateWindow::on_presplitMeshCheckBox_toggled(bool checked)
{
    m_presplitMesh = checked;
    apply();
}

void DecimateWindow::on_accumulateErrorCheckBox_toggled(bool checked)
{
    m_accumulateError = checked;
    apply();
}

void DecimateWindow::on_boundaryVertexDeletionCheckBox_toggled(bool checked)
{
    m_boundaryVertexDeletion = checked;
    apply();
}

void DecimateWindow::on_featureAngleSpinBox_valueChanged(double arg1)
{
    m_featureAngle = arg1;
    apply();
}

void DecimateWindow::on_splitAngleSpinBox_valueChanged(double arg1)
{
    m_splitAngle = arg1;
    apply();
}

void DecimateWindow::on_outputPointsPrecisionSpinBox_valueChanged(int arg1)
{
    m_outputPointsPrecision = arg1;
    apply();
}

void DecimateWindow::on_inflectionPointRatioSpinBox_valueChanged(double arg1)
{
    m_inflectionPointRatio = arg1;
    apply();
}

void DecimateWindow::on_degreeSpinBox_valueChanged(int arg1)
{
    m_degree = arg1;
    apply();
}

void DecimateWindow::on_openButton_clicked()
{
    browseFile();
    m_configUi->fileEdit->setText(fileName());
}

void DecimateWindow::createUi()
{
    m_configUi = new Ui::DecimateConfig;
    setupConfigWidget(m_configUi);

    // MUST be put at the end
    FilterWindow::createUi();
}
