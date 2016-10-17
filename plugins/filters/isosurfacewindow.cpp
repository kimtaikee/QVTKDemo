#include "isosurfacewindow.h"
#include "ui_isosurfaceconfig.h"
#include "ui_generalfilterwindow.h"

#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkPolyData.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkContourFilter.h>
#include <vtkLookupTable.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkLODActor.h>

#include <QWidget>
#include <QDebug>

IsosurfaceWindow::IsosurfaceWindow(QWidget *parent) :
    FilterWindow(parent)
{
    createUi();

    Utils::SignalBlocker sb(m_configUi->numOfContoursSpinBox);
    m_configUi->numOfContoursSpinBox->setValue(m_numOfContours);
    m_configUi->gradientCombo->setCurrentIndex(0);

    connect(m_configUi->minScalarSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBoxValueChanged(double)));
    connect(m_configUi->maxScalarSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBoxValueChanged(double)));
    connect(m_configUi->numOfContoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChanged(int)));
    connect(m_configUi->displayEffectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    setWindowTitle(tr("Isosurface"));
}

IsosurfaceWindow::~IsosurfaceWindow()
{

}

void IsosurfaceWindow::apply()
{
    if (!m_dataObject) {
        qDebug() << "Isosurface::apply: null data object.";
        return;
    }

    VTK_CREATE(vtkContourFilter, contourFilter);
    contourFilter->SetInputData(m_dataObject);
    contourFilter->ComputeScalarsOn();
    contourFilter->GenerateValues(m_numOfContours, m_minScalar, m_maxScalar);

    vtkSmartPointer<vtkLookupTable> lut = createLookupTable(m_minScalar, m_maxScalar);
    lut->SetNumberOfColors(m_numOfContours);
    lut->Build();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetLookupTable(lut);
    mapper->SetInputConnection(contourFilter->GetOutputPort());
    mapper->ScalarVisibilityOn();

    VtkUtils::vtkInitOnce(&m_filterActor);
    m_filterActor->SetMapper(mapper);

    m_vtkWidget->defaultRenderer()->AddActor(m_filterActor);

    applyDisplayEffect();
}

void IsosurfaceWindow::modelReady()
{
    FilterWindow::modelReady();

    vtkPolyData* polydata = vtkPolyData::SafeDownCast(m_dataObject);
    vtkDataSet* dataset = vtkDataSet::SafeDownCast(m_dataObject);
    QStringList scalarNames;
    double scalarRange[2];

    if (polydata) {
        // get scalar range
        polydata->GetScalarRange(scalarRange);
        // get scalar names from polydata
        vtkPointData* pointData = polydata->GetPointData();
        int numOfArray = pointData->GetNumberOfArrays();
        for (int i = 0; i < numOfArray; ++i)
            scalarNames.append(pointData->GetArrayName(i));
    } else if (dataset) {
        // get scalar range
        dataset->GetScalarRange(scalarRange);

        // get scalar names from dataset
        vtkCellData* cd = dataset->GetCellData();
        int arrayNum = cd->GetNumberOfArrays();
        if (arrayNum) {
            for (int i = 0; i < arrayNum; ++i)
                scalarNames.append(cd->GetArrayName(i));
        } else {
            vtkPointData* pd = dataset->GetPointData();
            arrayNum = pd->GetNumberOfArrays();
            for (int i = 0; i < arrayNum; ++i)
                scalarNames.append(pd->GetArrayName(i));
        }
    }

    setScalarRange(scalarRange[0], scalarRange[1]);

    m_configUi->scalarCombo->clear();
    m_configUi->scalarCombo->addItems(scalarNames);
    m_configUi->minScalarSpinBox->setRange(scalarRange[0], scalarRange[1]);
    m_configUi->minScalarSpinBox->setValue(scalarRange[0]);
    m_configUi->maxScalarSpinBox->setRange(scalarRange[0], scalarRange[1]);
    m_configUi->maxScalarSpinBox->setValue(scalarRange[1]);

//    showOrientationMarker();
    showScalarBar();

    vtkSmartPointer<vtkLookupTable> lut = createLookupTable(scalarRange[0], scalarRange[1]);
    lut->SetNumberOfColors(m_numOfContours);
    lut->Build();

    m_scalarBar->SetLookupTable(lut);
}

void IsosurfaceWindow::createUi()
{
    m_configUi = new Ui::IsosurfaceConfig;
    setupConfigWidget(m_configUi);

    FilterWindow::createUi();
}

void IsosurfaceWindow::colorsChanged()
{
    if (!m_filterActor || !m_scalarBar)
        return;

    vtkSmartPointer<vtkLookupTable> lut = createLookupTable(m_minScalar, m_maxScalar);
    lut->SetNumberOfColors(m_numOfContours);
    lut->Build();
    m_scalarBar->SetNumberOfLabels(m_numOfContours);
    m_filterActor->GetMapper()->SetLookupTable(lut);
    m_scalarBar->SetLookupTable(lut);
    m_vtkWidget->update();
}

void IsosurfaceWindow::on_openFileButton_clicked()
{
    browseFile();
    m_configUi->fileNameEdit->setText(m_fileName);
}

void IsosurfaceWindow::onDoubleSpinBoxValueChanged(double value)
{
    QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(sender());
    if (dsb == m_configUi->minScalarSpinBox)
        m_minScalar = value;
    else if (dsb == m_configUi->maxScalarSpinBox)
        m_maxScalar = value;

    apply();
}

void IsosurfaceWindow::onSpinBoxValueChanged(int value)
{
    m_numOfContours = value;
    apply();
}

void IsosurfaceWindow::onComboBoxIndexChanged(int index)
{
    QComboBox* cb = qobject_cast<QComboBox*>(sender());

    if (cb == m_configUi->displayEffectCombo)
        setDisplayEffect(static_cast<FilterWindow::DisplayEffect>(index));
    else if (cb == m_configUi->scalarCombo)
        int i = 0; // todo
}

void IsosurfaceWindow::on_gradientCombo_activated(int index)
{
    Q_UNUSED(index)
    Widgets::GradientComboBox* gcb = qobject_cast<Widgets::GradientComboBox*>(sender());
    setScalarBarColors(gcb->currentColor1(), gcb->currentColor2());
}
