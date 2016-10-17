#include "sliderwidgetwindow.h"
#include "ui_sliderwidgetconfig.h"
#include "ui_basewidgetwindow.h"

#include <utils/signalblocker.h>

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>
#include <vtkutils/sliderwidgetobserver.h>

#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkRenderer.h>

static const QString DefaultTitle = QObject::tr("Slider Bar");
static const QString DefaultFormat = "%6.3f";
SliderWidgetWindow::SliderWidgetWindow(QWidget* parent) : BaseWidgetWindow(parent)
{
    createUi();
    createWidget();
}

void SliderWidgetWindow::createWidget()
{
    VtkUtils::vtkInitOnce(m_2dRep);
    configRep(m_2dRep.GetPointer());
    VtkUtils::vtkInitOnce(m_2dWidget);
    hookWidget(m_2dWidget);
    m_2dWidget->SetAnimationModeToAnimate();
    m_2dWidget->SetRepresentation(m_2dRep);
    m_2dWidget->SetInteractor(m_vtkWidget->GetInteractor());
    m_2dWidget->On();
    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void SliderWidgetWindow::createUi()
{
    m_configUi = new Ui::SliderWidgetConfig;
    setupConfigWidget(m_configUi);

    Utils::SignalBlocker sb(m_configUi->titleEdit);
    sb.addObject(m_configUi->labelFormatEdit);

    m_configUi->titleEdit->setText(DefaultTitle);
    m_configUi->labelFormatEdit->setText(DefaultFormat);

    BaseWidgetWindow::createUi();

    setWindowTitle(tr("Slider Widget"));
}

void SliderWidgetWindow::on_typeCombo_currentIndexChanged(int index)
{
    if (index == 0) {
        m_2dWidget->On();

        if (m_3dWidget)
            m_3dWidget->Off();
    } else {
        if (!m_3dWidget) {
            VtkUtils::vtkInitOnce(m_3dRep);
            configRep(m_3dRep.GetPointer());
            VtkUtils::vtkInitOnce(m_3dWidget);
            hookWidget(m_3dWidget);
            m_3dWidget->SetRepresentation(m_3dRep);
            m_3dWidget->SetInteractor(m_vtkWidget->GetInteractor());
        }
        m_3dWidget->On();
        m_2dWidget->Off();
    }

    m_vtkWidget->update();
}

void SliderWidgetWindow::on_valueSpinBox_valueChanged(double arg1)
{
    if (m_2dWidget && m_2dWidget->GetEnabled())
        m_2dRep->SetValue(arg1);
    else if (m_3dWidget && m_3dWidget->GetEnabled())
        m_3dRep->SetValue(arg1);
    m_vtkWidget->update();
}

void SliderWidgetWindow::onValueChanged(double value)
{
    Utils::SignalBlocker sb(m_configUi->valueSpinBox);
    m_configUi->valueSpinBox->setValue(value);
}

template <class RepClass>
void SliderWidgetWindow::configRep(RepClass* rep)
{
    rep->SetTitleText(DefaultTitle.toUtf8().data());
    rep->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
    rep->GetPoint1Coordinate()->SetValue(-1,0,0);
    rep->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
    rep->GetPoint2Coordinate()->SetValue(1,0,0);
    rep->SetSliderLength(0.075);
    rep->SetSliderWidth(0.05);
    rep->SetEndCapLength(0.05);
    rep->SetMinimumValue(0);
    rep->SetMaximumValue(100);
    rep->SetLabelFormat(DefaultFormat.toUtf8().data());
}

void SliderWidgetWindow::hookWidget(const vtkSmartPointer<vtkSliderWidget>& widget)
{
    VtkUtils::SliderWidgetObserver* observer = new VtkUtils::SliderWidgetObserver(this);
    observer->attach(widget);
    connect(observer, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
}

void SliderWidgetWindow::on_titleEdit_textChanged(const QString &arg1)
{
    if (m_2dWidget && m_2dWidget->GetEnabled())
        m_2dRep->SetTitleText(arg1.toUtf8().data());
    else if (m_3dWidget && m_3dWidget->GetEnabled())
        m_3dRep->SetTitleText(arg1.toUtf8().data());
    m_vtkWidget->update();
}

void SliderWidgetWindow::on_labelFormatEdit_textChanged(const QString &arg1)
{
    if (m_2dWidget && m_2dWidget->GetEnabled())
        m_2dRep->SetLabelFormat(arg1.toUtf8().data());
    else if (m_3dWidget && m_3dWidget->GetEnabled())
        m_3dRep->SetLabelFormat(arg1.toUtf8().data());
    m_vtkWidget->update();
}
