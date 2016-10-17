#include "cutwindow.h"
#include "ui_cutconfig.h"
#include "ui_generalfilterwindow.h"

#include <utils/utils.h>
#include <utils/tablemodel.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/spherewidgetobserver.h>
#include <vtkutils/implicitplanewidgetobserver.h>
#include <vtkutils/boxwidgetobserver.h>

#include <vtkBoxWidget.h>
#include <vtkImplicitPlaneWidget.h>
#include <vtkSphereWidget.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkPlanes.h>
#include <vtkContourFilter.h>
#include <vtkRenderer.h>
#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkLookupTable.h>

#include <QWidget>
#include <QDebug>

CutWindow::CutWindow(QWidget* parent) : FilterWindow(parent)
{
    createUi();
    fireupModelToPointsConverter();
}

void CutWindow::setNormal(double normal[3])
{
    if (Utils::ArrayComparator<double>()(normal, m_normal))
        return;

    Utils::ArrayAssigner<double>()(m_normal, normal);
    apply();
}

void CutWindow::setOrigin(double origin[3])
{
    if (Utils::ArrayComparator<double>()(origin, m_normal))
        return;

    Utils::ArrayAssigner<double>()(m_normal, origin);
    apply();
}

void CutWindow::setRadius(double radius)
{

}

void CutWindow::setCutType(CutType type)
{
    if (m_cutType != type) {
        m_cutType = type;
        updateCutWidget();
    }
}

CutWindow::CutType CutWindow::cutType() const
{
    return m_cutType;
}

void CutWindow::updateCutWidget()
{
    double bounds[6];
    if (isValidPolyData()) {
        vtkPolyData* polyData = vtkPolyData::SafeDownCast(m_dataObject);
        polyData->GetBounds(bounds);
    } else if (isValidDataSet()) {
        vtkDataSet* dataSet = vtkDataSet::SafeDownCast(m_dataObject);
        dataSet->GetBounds(bounds);
    }

    switch (m_cutType) {
    case Plane:
    {
        if (!m_planeWidget) {
            m_planeWidget = vtkImplicitPlaneWidget::New();
            m_planeWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_planeWidget->SetPlaceFactor(1);
            m_planeWidget->SetProp3D(m_modelActor);
            m_planeWidget->SetOutlineTranslation(0); // make the outline non-movable
            m_planeWidget->GetPlaneProperty()->SetOpacity(0.7);
            m_planeWidget->GetPlaneProperty()->SetColor(.0, .0, .0);

            VtkUtils::ImplicitPlaneWidgetObserver* observer = new VtkUtils::ImplicitPlaneWidgetObserver(this);
            connect(observer, SIGNAL(originChanged(double*)), this, SLOT(onOriginChanged(double*)));
            connect(observer, SIGNAL(normalChanged(double*)), this, SLOT(onNormalChanged(double*)));
            observer->attach(m_planeWidget);

            resetPlaneWidget();
        }

        m_planeWidget->On();
        m_planeWidget->SetOrigin(m_origin);
        m_planeWidget->SetNormal(m_normal);
        m_planeWidget->PlaceWidget(bounds);

        safeOff(m_sphereWidget);
        safeOff(m_boxWidget);

        m_configUi->planeGroupBox->show();
        m_configUi->sphereGroupBox->hide();
    }
        break;

    case Sphere:
    {
        if (!m_sphereWidget) {
            VtkUtils::vtkInitOnce(m_sphereWidget);
            m_sphereWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_sphereWidget->SetProp3D(m_modelActor);
            m_sphereWidget->PlaceWidget();

            VtkUtils::SphereWidgetObserver* observer = new VtkUtils::SphereWidgetObserver(this);
            observer->attach(m_sphereWidget);
            connect(observer, SIGNAL(centerChanged(double*)), this, SLOT(onCenterChanged(double*)));
        }

        m_sphereWidget->PlaceWidget(bounds);
        m_sphereWidget->SetCenter(m_center);
        m_sphereWidget->SetRadius(m_radius);
        m_sphereWidget->On();
        safeOff(m_planeWidget);
        safeOff(m_boxWidget);

        m_configUi->planeGroupBox->hide();
        m_configUi->sphereGroupBox->show();

    }
        break;

    case Box:
    {
        if (!m_boxWidget) {
            m_boxWidget = vtkBoxWidget::New();
            VtkUtils::vtkInitOnce(m_boxWidget);
            m_boxWidget->SetInteractor(m_vtkWidget->GetInteractor());
            m_boxWidget->SetPlaceFactor(1.0);

            m_boxWidget->SetProp3D(m_modelActor);
            m_boxWidget->PlaceWidget();

            VtkUtils::BoxWidgetObserver* observer = new VtkUtils::BoxWidgetObserver(this);
            observer->attach(m_boxWidget);
            connect(observer, SIGNAL(planesChanged(vtkPlanes*)), this, SLOT(onPlanesChanged(vtkPlanes*)));
        }

        m_boxWidget->On();
        m_boxWidget->PlaceWidget(bounds);
        safeOff(m_planeWidget);
        safeOff(m_sphereWidget);

        m_configUi->planeGroupBox->hide();
        m_configUi->sphereGroupBox->hide();

    }
        break;
    }
}

void CutWindow::onOriginChanged(double* origin)
{
    if (Utils::ArrayComparator<double>()(m_origin, origin))
        return;

    Utils::ArrayAssigner<double>()(m_origin, origin);

    Utils::SignalBlocker sb(m_configUi->originXSpinBox);
    sb.addObject(m_configUi->originYSpinBox);
    sb.addObject(m_configUi->originZSpinBox);

    m_configUi->originXSpinBox->setValue(origin[0]);
    m_configUi->originYSpinBox->setValue(origin[1]);
    m_configUi->originZSpinBox->setValue(origin[2]);

    apply();
}

void CutWindow::onNormalChanged(double* normal)
{
    if (Utils::ArrayComparator<double>()(m_normal, normal))
        return;

    Utils::ArrayAssigner<double>()(m_normal, normal);

    Utils::SignalBlocker sb(m_configUi->normalXSpinBox);
    sb.addObject(m_configUi->normalYSpinBox);
    sb.addObject(m_configUi->normalZSpinBox);

    m_configUi->normalXSpinBox->setValue(normal[0]);
    m_configUi->normalYSpinBox->setValue(normal[1]);
    m_configUi->normalZSpinBox->setValue(normal[2]);

    apply();
}

void CutWindow::onCenterChanged(double* center)
{
    if (Utils::ArrayComparator<double>()(m_center, center))
        return;

    Utils::ArrayAssigner<double>()(m_center, center);

    Utils::SignalBlocker sb(m_configUi->centerXSpinBox);
    sb.addObject(m_configUi->centerYSpinBox);
    sb.addObject(m_configUi->centerZSpinBox);

    m_configUi->centerXSpinBox->setValue(center[0]);
    m_configUi->centerYSpinBox->setValue(center[1]);
    m_configUi->centerZSpinBox->setValue(center[2]);

    apply();
}

void CutWindow::onPlanesChanged(vtkPlanes* planes)
{
    m_planes = planes;
    apply();
}

void CutWindow::showContourLines(bool show)
{
    if (!show && !m_resultData)
        return;

    if (!show && m_contourLinesActor) {
        m_contourLinesActor->SetVisibility(show);
        m_vtkWidget->update();
        return;
    }

    vtkPolyData* polyData = vtkPolyData::SafeDownCast(m_resultData);
    if (!polyData)
        return;

    vtkPointData* pointData = polyData->GetPointData();

    // store contour names
    int numOfArrays = pointData->GetNumberOfArrays();
    if (!numOfArrays)
        return;

    //
    QMap<QString, vtkDataArray*> scalarNameDataMap;
    QString contourName;
    for (int i = 0; i < numOfArrays; ++i) {
        contourName = pointData->GetArrayName(i);
        scalarNameDataMap.insert(pointData->GetArrayName(i), pointData->GetArray(pointData->GetArrayName(i)));
    }

    //
    VTK_CREATE(vtkDoubleArray, scalars);
    double range[2];

//    if (d_ptr->contourNameList.isEmpty())
//        for (int i = 0; i < numOfArrays; ++i)
//            if (pointData->GetArray(i)->GetNumberOfTuples() > 0) // skip empty data array
//                d_ptr->contourNameList.append(pointData->GetArrayName(i));
//    d_ptr->contourNameList.removeAll(QString());

    // now extracting contour data
    for (int i = 0; i < numOfArrays; ++i) {
//        if (d_ptr->contourName.isEmpty())
//            d_ptr->contourName = pointData->GetArrayName(i);

        vtkDataArray* da = pointData->GetArray(contourName.toUtf8().data());
        if (da) {
            int numOfTuple = da->GetNumberOfTuples();
            da->GetRange(range);
            for (int j = 0; j < numOfTuple; ++j) {
                double val = da->GetTuple1(j);
                scalars->InsertNextTuple1(val);
            }
            break;
        }
    }

    /// START
    polyData->GetPointData()->SetScalars(scalars);
    VTK_CREATE(vtkAppendPolyData, appendFilledContours);
    int numberOfContours = 10;

    double delta = (range[1] - range[0]) / static_cast<double> (numberOfContours - 1);

    // Keep the clippers alive
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersLo;
    std::vector<vtkSmartPointer<vtkClipPolyData> > clippersHi;

    for (int i = 0; i < numberOfContours; i++) {
        double valueLo = range[0] + static_cast<double> (i) * delta;
        double valueHi = range[0] + static_cast<double> (i + 1) * delta;
        clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersLo[i]->SetValue(valueLo);
        if (i == 0)
            clippersLo[i]->SetInputData(polyData);
        else
            clippersLo[i]->SetInputConnection(clippersHi[i - 1]->GetOutputPort(1));
        clippersLo[i]->InsideOutOff();
        clippersLo[i]->Update();

        clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersHi[i]->SetValue(valueHi);
        clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
        clippersHi[i]->GenerateClippedOutputOn();
        clippersHi[i]->InsideOutOn();
        clippersHi[i]->Update();
        if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0)
            continue;

        VTK_CREATE(vtkFloatArray, cd);
        cd->SetNumberOfComponents(1);
        cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
        cd->FillComponent(0, valueLo);

        clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
        appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
    }

    VTK_CREATE(vtkCleanPolyData, filledContours);
    filledContours->SetInputConnection(appendFilledContours->GetOutputPort());

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetNumberOfColors(numberOfContours);
    lut->SetRange(range[0], range[1]);
    lut->SetHueRange(0.6667, 0);
    lut->Build();

    VTK_CREATE(vtkPolyDataMapper, contourMapper);
    contourMapper->SetInputConnection(filledContours->GetOutputPort());
    contourMapper->SetScalarRange(range[0], range[1]);
    contourMapper->SetScalarModeToUseCellData();
    contourMapper->SetLookupTable(lut);

    if (!m_contourLinesActor)
        m_contourLinesActor = vtkActor::New();
    m_contourLinesActor->SetMapper(contourMapper);
    m_contourLinesActor->SetVisibility(show);
    m_vtkWidget->defaultRenderer()->AddActor(m_contourLinesActor);
    m_vtkWidget->update();
}

void CutWindow::modelReady()
{
    FilterWindow::modelReady();

    // initialize vars
    double bounds[6];
    double center[3];
    double scalarRange[2];
    if (isValidPolyData()) {
        vtkPolyData* polyData = vtkPolyData::SafeDownCast(m_dataObject);
        polyData->GetBounds(bounds);
        polyData->GetCenter(center);
        polyData->GetScalarRange(scalarRange);
    } else if (isValidDataSet()) {
        vtkDataSet* dataSet = vtkDataSet::SafeDownCast(m_dataObject);
        dataSet->GetBounds(bounds);
        dataSet->GetCenter(center);
        dataSet->GetScalarRange(scalarRange);
    }

    setScalarRange(scalarRange[0], scalarRange[1]);

    double xRange = bounds[1] - bounds[0];
    double yRange = bounds[3] - bounds[2];
    double zRange = bounds[5] - bounds[4];
    double minRange = qMin(xRange, qMin(yRange, zRange));
    m_radius = (double)minRange / 2;

    Utils::ArrayAssigner<double>()(m_center, center);
    Utils::ArrayAssigner<double>()(m_origin, center);

    m_normal[0] = 0;
    m_normal[1] = 0;
    m_normal[2] = 1;

    Utils::SignalBlocker sb(m_configUi->centerXSpinBox);
    sb.addObject(m_configUi->centerYSpinBox);
    sb.addObject(m_configUi->centerZSpinBox);
    sb.addObject(m_configUi->originZSpinBox);
    sb.addObject(m_configUi->originYSpinBox);
    sb.addObject(m_configUi->originZSpinBox);
    sb.addObject(m_configUi->radiusSpinBox);

    m_configUi->centerXSpinBox->setValue(center[0]);
    m_configUi->centerYSpinBox->setValue(center[1]);
    m_configUi->centerZSpinBox->setValue(center[2]);
    m_configUi->originXSpinBox->setValue(center[0]);
    m_configUi->originYSpinBox->setValue(center[1]);
    m_configUi->originZSpinBox->setValue(center[2]);
    m_configUi->radiusSpinBox->setValue(m_radius);

    showScalarBar();
    updateCutWidget();
}

void CutWindow::createUi()
{
    m_configUi = new Ui::CutConfig;
    setupConfigWidget(m_configUi);
    m_configUi->gradientCombo->setCurrentIndex(0);

    // must be put at the end
    FilterWindow::createUi();

    Utils::SignalBlocker sb(m_configUi->rowsSpinBox);
    m_configUi->rowsSpinBox->setValue(DefaultRows);
    m_configUi->sphereGroupBox->hide();

    initTableModel();
    m_configUi->dataTable->setModel(m_tableModel);
}

void CutWindow::dataChanged()
{
    switch (m_cutType) {
    case Plane:
        resetPlaneWidget();
        break;

    case Sphere:
        resetSphereWidget();
        break;

    case Box:
        resetBoxWidget();
        break;
    }

    apply();
}

void CutWindow::safeOff(vtk3DWidget* widget)
{
    if (widget)
        widget->Off();
}

void CutWindow::resetPlaneWidget()
{
    if (!m_planeWidget) {
        qDebug() << "CutWindow::resetPlaneWidget: null plane widget.";
        return;
    }

    double bounds[6];
    double origin[3];
    if (isValidPolyData()) {
        vtkPolyData* polyData = vtkPolyData::SafeDownCast(m_dataObject);
        polyData->GetBounds(bounds);
        polyData->GetCenter(origin);
    } else if (isValidDataSet()) {
        vtkDataSet* dataSet = vtkDataSet::SafeDownCast(m_dataObject);
        dataSet->GetBounds(bounds);
        dataSet->GetCenter(origin);
    }

    Utils::ArrayAssigner<double>()(m_origin, origin);
    m_planeWidget->PlaceWidget(bounds);
}

void CutWindow::resetSphereWidget()
{

}

void CutWindow::resetBoxWidget()
{

}

void CutWindow::on_cutTypeCombo_currentIndexChanged(int index)
{
    setCutType(static_cast<CutType>(index));
    apply();
}

void CutWindow::on_displayEffectCombo_currentIndexChanged(int index)
{
    setDisplayEffect(static_cast<DisplayEffect>(index));
}

void CutWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    m_tableModel->resize(m_tableModel->columnCount(), arg1);
    m_tableModel->random();
    fireupModelToPointsConverter();
}

void CutWindow::on_radiusSpinBox_valueChanged(double arg1)
{
    m_radius = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_originXSpinBox_valueChanged(double arg1)
{
    m_origin[0] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_originYSpinBox_valueChanged(double arg1)
{
    m_origin[1] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_originZSpinBox_valueChanged(double arg1)
{
    m_origin[2] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_normalXSpinBox_valueChanged(double arg1)
{
    m_normal[0] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_normalYSpinBox_valueChanged(double arg1)
{
    m_normal[1] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_normalZSpinBox_valueChanged(double arg1)
{
    m_normal[2] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_centerXSpinBox_valueChanged(double arg1)
{
    m_center[0] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_centerYSpinBox_valueChanged(double arg1)
{
    m_center[1] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_centerZSpinBox_valueChanged(double arg1)
{
    m_center[2] = arg1;
    updateCutWidget();
    apply();
}

void CutWindow::on_openFileButton_clicked()
{
    browseFile();
    m_configUi->fileEdit->setText(m_fileName);
}

void CutWindow::on_randomButton_clicked()
{
    randomTableModel();
}

void CutWindow::on_showPlaneCheckBox_toggled(bool checked)
{
    if (m_planeWidget)
        m_planeWidget->SetDrawPlane(checked);
}

void CutWindow::on_showContourLinesCheckBox_toggled(bool checked)
{
    showContourLines(checked);
}

void CutWindow::on_gradientCombo_activated(int index)
{
    Q_UNUSED(index)
    Widgets::GradientComboBox* gcb = qobject_cast<Widgets::GradientComboBox*>(sender());
    setScalarBarColors(gcb->currentColor1(), gcb->currentColor2());
}

