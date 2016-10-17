#ifndef SLIDERWIDGETWINDOW_H
#define SLIDERWIDGETWINDOW_H

#include "basewidgetwindow.h"

namespace Ui
{
    class SliderWidgetConfig;
}

class vtkSliderWidget;
class vtkSliderRepresentation;
class vtkSliderRepresentation2D;
class vtkSliderRepresentation3D;
class SliderWidgetWindow : public BaseWidgetWindow
{
    Q_OBJECT
public:
    explicit SliderWidgetWindow(QWidget* parent = nullptr);

protected:
    void createWidget();
    void createUi();

private slots:
    void on_typeCombo_currentIndexChanged(int index);
    void on_valueSpinBox_valueChanged(double arg1);
    void onValueChanged(double value);
    void on_titleEdit_textChanged(const QString &arg1);
    void on_labelFormatEdit_textChanged(const QString &arg1);

private:
    template <class RepClass>
    void configRep(RepClass* rep);

    void hookWidget(const vtkSmartPointer<vtkSliderWidget>& widget);

private:
    vtkSmartPointer<vtkSliderWidget> m_2dWidget;
    vtkSmartPointer<vtkSliderWidget> m_3dWidget;
    vtkSmartPointer<vtkSliderRepresentation2D> m_2dRep;
    vtkSmartPointer<vtkSliderRepresentation3D> m_3dRep;
    Ui::SliderWidgetConfig* m_configUi = nullptr;
};

#endif // SLIDERWIDGETWINDOW_H
