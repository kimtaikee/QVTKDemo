#include "ui_romanconfig.h"
#include "ui_randomhillsconfig.h"
#include "ui_ellipsoidconfig.h"
#include "ui_diniconfig.h"
#include "ui_conicspiralconfig.h"
#include "ui_parametricobjectswindow.h"
#include "ui_splineconfig.h"
#include "ui_superellipsoidconfig.h"
#include "ui_supertoroidconfig.h"
#include "ui_torusconfig.h"
#include "parametricobjectswindow.h"

#include <utils/tablemodel.h>

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricBoy.h>
#include <vtkParametricConicSpiral.h>
#include <vtkParametricCrossCap.h>
#include <vtkParametricDini.h>
#include <vtkParametricEllipsoid.h>
#include <vtkParametricEnneper.h>
#include <vtkParametricFigure8Klein.h>
#include <vtkParametricKlein.h>
#include <vtkParametricMobius.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricRoman.h>
#include <vtkParametricSpline.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkParametricSuperToroid.h>
#include <vtkParametricTorus.h>

ParametricObjectsWindow::ParametricObjectsWindow(QWidget *parent) : QWidget(parent), m_ui(new Ui::ParametricObjectsWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Parametrics Objects"));

    m_dummyWidget = new QWidget(this);
    m_ui->stackWidget->addWidget(m_dummyWidget);

    m_vtkWidget = new VtkUtils::VtkWidget(this);
    m_ui->previewLayout->addWidget(m_vtkWidget);

    vtkSmartPointer<vtkParametricFunction> parametricObject;
    VTK_CREATE(vtkParametricFunctionSource, parametricFunctionSource);
    parametricFunctionSource->SetParametricFunction(parametricObject);
    parametricFunctionSource->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(parametricFunctionSource->GetOutputPort());

    VtkUtils::vtkInitOnce(m_objectActor);
    m_objectActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_objectActor);
    m_vtkWidget->update();

    on_comboBox_currentIndexChanged(0);
}

void ParametricObjectsWindow::on_comboBox_currentIndexChanged(int index)
{
    createConfigWidget(index);

    VTK_CREATE(vtkParametricFunctionSource, parametricFunctionSource);
    vtkSmartPointer<vtkParametricFunction> parametricObject;
    switch (index) {
    case 0:
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(vtkParametricBoy::New());
        break;

    case 1:
    {
        vtkParametricConicSpiral* conicSpiral = vtkParametricConicSpiral::New();
        conicSpiral->SetA(m_conicSpiralA);
        conicSpiral->SetB(m_conicSpiralB);
        conicSpiral->SetC(m_conicSpiralC);
        conicSpiral->SetN(m_conicSpiralN);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(conicSpiral);
    }
        break;

    case 2:
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(vtkParametricCrossCap::New());
        break;

    case 3:
    {
        vtkParametricDini* dini = vtkParametricDini::New();
        dini->SetA(m_diniA);
        dini->SetB(m_diniB);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(dini);
    }
        break;

    case 4:
    {
        vtkParametricEllipsoid* ellipsoid = vtkParametricEllipsoid::New();
        ellipsoid->SetXRadius(m_ellipsoidXRadius);
        ellipsoid->SetYRadius(m_ellipsoidYRadius);
        ellipsoid->SetZRadius(m_ellipsoidZRadius);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(ellipsoid);
    }
        break;

    case 5:
        parametricObject = vtkParametricEnneper::New();
        break;

    case 6:
    {
        vtkParametricFigure8Klein* klein = vtkParametricFigure8Klein::New();
        klein->SetRadius(m_romanRadius);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(klein);
    }
        break;

    case 7:
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(vtkParametricKlein::New());
        break;

    case 8:
    {
        vtkParametricMobius* mobius = vtkParametricMobius::New();
        mobius->SetRadius(m_romanRadius);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(mobius);
    }
        break;

    case 9:
    {
        vtkParametricRandomHills* hills = vtkParametricRandomHills::New();
        hills->SetNumberOfHills(m_randomHillsNumOfHills);
        hills->SetHillXVariance(m_randomHillsXVar);
        hills->SetHillYVariance(m_randomHillsYVar);
        hills->SetHillAmplitude(m_randomHillsAmp);
        hills->SetRandomSeed(m_randomHillsRandomSeed);
        hills->SetAllowRandomGeneration(m_randomHillsRandomGen);
        hills->SetXVarianceScaleFactor(m_randomHillsXVarScaleFactor);
        hills->SetYVarianceScaleFactor(m_randomHillsYVarScaleFactor);
        hills->SetAmplitudeScaleFactor(m_randomHillsAmpScaleFactor);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(hills);
    }
        break;

    case 10:
    {
        vtkParametricRoman* roman = vtkParametricRoman::New();
        roman->SetRadius(m_romanRadius);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(roman);
    }
        break;
    case 11:
    {
        vtkParametricSpline* spline = vtkParametricSpline::New();
        vtkSmartPointer<vtkPoints> points = this->pointsFromTable();
        spline->SetPoints(points);
        spline->SetClosed(m_splineClosed);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(spline);
    }
        break;

    case 12:
    {
        vtkParametricSuperEllipsoid* superEllisoid = vtkParametricSuperEllipsoid::New();
        superEllisoid->SetXRadius(m_superEllipsoidXRadius);
        superEllisoid->SetYRadius(m_superEllipsoidYRadius);
        superEllisoid->SetZRadius(m_superEllipsoidZRadius);
        superEllisoid->SetN1(m_superEllipsoidN1);
        superEllisoid->SetN2(m_superEllipsoidN2);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(superEllisoid);
    }
        break;

    case 13:
    {
        vtkParametricSuperToroid* superToriod = vtkParametricSuperToroid::New();
        superToriod->SetXRadius(m_superToroidXRadius);
        superToriod->SetYRadius(m_superToroidYRadius);
        superToriod->SetZRadius(m_superToroidZRadius);
        superToriod->SetN1(m_superToroidN1);
        superToriod->SetN2(m_superToroidN2);
        superToriod->SetRingRadius(m_superToroidRingRadius);
        superToriod->SetCrossSectionRadius(m_superToroidCrossSectionRadius);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(superToriod);
    }
        break;

    case 14:
    {
        vtkParametricTorus* torus = vtkParametricTorus::New();
        torus->SetRingRadius(m_torusRingRadius);
        torus->SetCrossSectionRadius(m_torusCrossSectionRadius);
        parametricObject = vtkSmartPointer<vtkParametricFunction>::Take(torus);
    }
        break;
    }

    parametricFunctionSource->SetParametricFunction(parametricObject);
    parametricFunctionSource->Update();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(parametricFunctionSource->GetOutputPort());

    m_objectActor->SetMapper(mapper);
    m_vtkWidget->update();
    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void ParametricObjectsWindow::on_romanRadiusSpinBox_valueChanged(double value)
{
    m_romanRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_ellipsoidXRadiusSpinBox_valueChanged(double value)
{
    m_ellipsoidXRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_ellipsoidYRadiusSpinBox_valueChanged(double value)
{
    m_ellipsoidYRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_ellipsoidZRadiusSpinBox_valueChanged(double value)
{
    m_ellipsoidZRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_diniASpinBox_valueChanged(double value)
{
    m_diniA = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_diniBSpinBox_valueChanged(double value)
{
    m_diniB = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_conicSpiralASpinBox_valueChanged(double value)
{
    m_conicSpiralA = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_conicSpiralBSpinBox_valueChanged(double value)
{
    m_conicSpiralB = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_conicSpiralCSpinBox_valueChanged(double value)
{
    m_conicSpiralC = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_conicSpiralNSpinBox_valueChanged(double value)
{
    m_conicSpiralN = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_numOfHills_valueChanged(int value)
{
    m_randomHillsNumOfHills = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_xVariance_valueChanged(double value)
{
    m_randomHillsXVar = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_yVariance_valueChanged(double value)
{
    m_randomHillsYVar = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_amplitude_valueChanged(double value)
{
    m_randomHillsAmp = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_randomSeed_valueChanged(int value)
{
    m_randomHillsRandomSeed = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_randomGen_toggled(bool toggled)
{
    m_randomHillsRandomGen = toggled;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_xVarScaleFactor_valueChanged(double value)
{
    m_randomHillsXVarScaleFactor = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_yVarScaleFactor_valueChanged(double value)
{
    m_randomHillsYVarScaleFactor = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_ampScaleFactor_valueChanged(double value)
{
    m_randomHillsAmpScaleFactor = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_dataTable_dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_rowsSpinBox_valueChanged(int value)
{
    m_tableModel->resize(3, value);
    m_tableModel->random();
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_randomButton_clicked()
{
    m_tableModel->random();
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_closedCheckBox_toggled(bool toggle)
{
    m_splineClosed = toggle;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_xRadiusSpinBox_valueChanged(double value)
{
    m_superEllipsoidXRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_yRadiusSpinBox_valueChanged(double value)
{
    m_superEllipsoidYRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_zRadiusSpinBox_valueChanged(double value)
{
    m_superEllipsoidZRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_n1SpinBox_valueChanged(double value)
{
    m_superEllipsoidN1 = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_n2SpinBox_valueChanged(double value)
{
    m_superEllipsoidN2 = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_xRadiusSpinBox_valueChanged_superToroid(double value)
{
    m_superToroidXRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_yRadiusSpinBox_valueChanged_superToroid(double value)
{
    m_superToroidYRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_zRadiusSpinBox_valueChanged_superToroid(double value)
{
    m_superToroidZRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_n1SpinBox_valueChanged_superToroid(double value)
{
    m_superToroidN1 = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_n2SpinBox_valueChanged_superToroid(double value)
{
    m_superToroidN2 = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_ringRadiusSpinBox_valueChanged(double value)
{
    m_superToroidRingRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_crossSectionRadiusSpinBox_valueChanged(double value)
{
    m_superToroidCrossSectionRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_ringRadiusSpinBox_valueChanged_torus(double value)
{
    m_torusRingRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::on_crossSectionRadiusSpinBox_valueChanged_torus(double value)
{
    m_torusCrossSectionRadius = value;
    on_comboBox_currentIndexChanged(m_ui->comboBox->currentIndex());
}

void ParametricObjectsWindow::createConfigWidget(int func)
{
    switch (func) {
    case 1:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::ConicSpiralConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 3:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::DiniConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 4:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::EllipsoidConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 9:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::RandomHillsConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 8:
    case 10:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::RomanConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 11:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::SplineConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 12:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::SuperEllipsoidConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 13:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::SuperToroidConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;

    case 14:
        if (!m_funcWidgetMap.contains(func)) {
            QWidget* widget = createConfigWidgetTempl<Ui::TorusConfig>();
            m_ui->stackWidget->addWidget(widget);
            m_funcWidgetMap.insert(func, widget);
        }
        break;
    }

    QWidget* widget = m_funcWidgetMap.value(func);
    if (widget)
        m_ui->stackWidget->setCurrentWidget(widget);
    else
        m_ui->stackWidget->setCurrentWidget(m_dummyWidget);
}

vtkSmartPointer<vtkPoints> ParametricObjectsWindow::pointsFromTable()
{
    VTK_CREATE(vtkPoints, points);
    int rows = m_tableModel->rowCount();
    double x, y, z;
    for (int i = 0; i < rows; ++i) {
        x = m_tableModel->data(i, 0);
        y = m_tableModel->data(i, 1);
        z = m_tableModel->data(i, 2);
        points->InsertNextPoint(x, y, z);
    }
    return points;
}


template <class UiClass>
QWidget* ParametricObjectsWindow::createConfigWidgetTempl()
{
    UiClass* configUi = new UiClass;
    QWidget* configWidget = new QWidget(this);
    configUi->setupUi(configWidget);
    createConfigWidgetConns(configUi);
    return configWidget;
}

template <class UiClass>
void ParametricObjectsWindow::createConfigWidgetConns(UiClass* ui)
{
    // no impl
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::RomanConfig>(Ui::RomanConfig *ui)
{
    ui->radiusSpinBox->setValue(m_romanRadius);
    connect(ui->radiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_romanRadiusSpinBox_valueChanged(double)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::EllipsoidConfig>(Ui::EllipsoidConfig* ui)
{
    ui->xRadiusSpinBox->setValue(m_ellipsoidXRadius);
    ui->yRadiusSpinBox->setValue(m_ellipsoidYRadius);
    ui->zRadiusSpinBox->setValue(m_ellipsoidZRadius);
    connect(ui->xRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_ellipsoidXRadiusSpinBox_valueChanged(double)));
    connect(ui->yRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_ellipsoidYRadiusSpinBox_valueChanged(double)));
    connect(ui->zRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_ellipsoidZRadiusSpinBox_valueChanged(double)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::DiniConfig>(Ui::DiniConfig* ui)
{
    ui->aSpinBox->setValue(m_diniA);
    ui->bSpinBox->setValue(m_diniB);
    connect(ui->aSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_diniASpinBox_valueChanged(double)));
    connect(ui->bSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_diniBSpinBox_valueChanged(double)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::ConicSpiralConfig>(Ui::ConicSpiralConfig* ui)
{
    ui->aSpinBox->setValue(m_conicSpiralA);
    ui->bSpinBox->setValue(m_conicSpiralB);
    ui->cSpinBox->setValue(m_conicSpiralC);
    ui->nSpinBox->setValue(m_conicSpiralN);
    connect(ui->aSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_conicSpiralASpinBox_valueChanged(double)));
    connect(ui->bSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_conicSpiralBSpinBox_valueChanged(double)));
    connect(ui->cSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_conicSpiralCSpinBox_valueChanged(double)));
    connect(ui->nSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_conicSpiralNSpinBox_valueChanged(double)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::RandomHillsConfig>(Ui::RandomHillsConfig* ui)
{
    ui->numOfHillsSpinBox->setValue(m_randomHillsNumOfHills);
    ui->hillXVarSpinBox->setValue(m_randomHillsXVar);
    ui->hillYVarSpinBox->setValue(m_randomHillsYVar);
    ui->hillAmplitudeSpinBox->setValue(m_randomHillsAmp);
    ui->randomSeedSpinBox->setValue(m_randomHillsRandomSeed);
    ui->allowRandGenCheckBox->setChecked(m_randomHillsRandomGen);
    ui->xVarScaleFactorSpinBox->setValue(m_randomHillsXVarScaleFactor);
    ui->yVarScaleFactorSpinBox->setValue(m_randomHillsYVarScaleFactor);
    ui->ampScaleFactorSpinBox->setValue(m_randomHillsAmpScaleFactor);
    connect(ui->numOfHillsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_numOfHills_valueChanged(int)));
    connect(ui->hillXVarSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_xVariance_valueChanged(double)));
    connect(ui->hillYVarSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_yVariance_valueChanged(double)));
    connect(ui->hillAmplitudeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_amplitude_valueChanged(double)));
    connect(ui->randomSeedSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_randomSeed_valueChanged(int)));
    connect(ui->allowRandGenCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_randomGen_toggled(bool)));
    connect(ui->xVarScaleFactorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_xVarScaleFactor_valueChanged(double)));
    connect(ui->yVarScaleFactorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_yVarScaleFactor_valueChanged(double)));
    connect(ui->ampScaleFactorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_ampScaleFactor_valueChanged(double)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::SplineConfig>(Ui::SplineConfig* ui)
{
    ui->rowsSpinBox->setValue(10);
    m_tableModel = new Utils::TableModel(3, 10, this);
    m_tableModel->setHorizontalHeaderData(QVariantList() << "X" << "Y" << "Z");
    ui->dataTable->setModel(m_tableModel);
    m_tableModel->random();

    connect(ui->rowsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_rowsSpinBox_valueChanged(int)));
    connect(ui->randomButton, SIGNAL(clicked()), this, SLOT(on_randomButton_clicked()));
    connect(ui->closedCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_closedCheckBox_toggled(bool)));
    connect(m_tableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(on_dataTable_dataChanged(QModelIndex,QModelIndex)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::SuperEllipsoidConfig>(Ui::SuperEllipsoidConfig* ui)
{
    ui->xRadiusSpinBox->setValue(m_superEllipsoidXRadius);
    ui->yRadiusSpinBox->setValue(m_superEllipsoidYRadius);
    ui->zRadiusSpinBox->setValue(m_superEllipsoidZRadius);
    ui->n1SpinBox->setValue(m_superEllipsoidN1);
    ui->n2SpinBox->setValue(m_superEllipsoidN2);

    connect(ui->xRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_xRadiusSpinBox_valueChanged(double)));
    connect(ui->yRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_yRadiusSpinBox_valueChanged(double)));
    connect(ui->zRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_zRadiusSpinBox_valueChanged(double)));
    connect(ui->n1SpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_n1SpinBox_valueChanged(double)));
    connect(ui->n2SpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_n2SpinBox_valueChanged(double)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::SuperToroidConfig>(Ui::SuperToroidConfig* ui)
{
    ui->xRadiusSpinBox->setValue(m_superToroidXRadius);
    ui->yRadiusSpinBox->setValue(m_superToroidYRadius);
    ui->zRadiusSpinBox->setValue(m_superToroidZRadius);
    ui->n1SpinBox->setValue(m_superToroidN1);
    ui->n2SpinBox->setValue(m_superToroidN2);
    ui->ringRadiusSpinBox->setValue(m_superToroidRingRadius);
    ui->crossSectionRadiusSpinBox->setValue(m_superToroidCrossSectionRadius);

    connect(ui->xRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_xRadiusSpinBox_valueChanged_superToroid(double)));
    connect(ui->yRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_yRadiusSpinBox_valueChanged_superToroid(double)));
    connect(ui->zRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_zRadiusSpinBox_valueChanged_superToroid(double)));
    connect(ui->n1SpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_n1SpinBox_valueChanged_superToroid(double)));
    connect(ui->n2SpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_n2SpinBox_valueChanged_superToroid(double)));
    connect(ui->ringRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_ringRadiusSpinBox_valueChanged(double)));
    connect(ui->crossSectionRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_crossSectionRadiusSpinBox_valueChanged(double)));
}

template <>
void ParametricObjectsWindow::createConfigWidgetConns<Ui::TorusConfig>(Ui::TorusConfig* ui)
{
    ui->ringRadiusSpinBox->setValue(m_torusRingRadius);
    ui->crossSectionRadiusSpinBox->setValue(m_torusCrossSectionRadius);

    connect(ui->ringRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_ringRadiusSpinBox_valueChanged_torus(double)));
    connect(ui->crossSectionRadiusSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_crossSectionRadiusSpinBox_valueChanged_torus(double)));
}



