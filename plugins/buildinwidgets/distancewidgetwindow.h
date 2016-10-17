#ifndef DISTANCEWIDGETWINDOW_H
#define DISTANCEWIDGETWINDOW_H

#include "basewidgetwindow.h"

namespace Ui
{
    class DistanceWidgetConfig;
}

class vtkDistanceWidget;
class vtkDistanceRepresentation2D;
class vtkDistanceRepresentation3D;
class DistanceWidgetWindow : public BaseWidgetWindow
{
    Q_OBJECT
public:
    explicit DistanceWidgetWindow(QWidget* parent = nullptr);

protected:
    void createWidget();
    void createUi();

private slots:
    void on_typeCombo_currentIndexChanged(int index);
    void on_point1XSpinBox_valueChanged(double arg1);
    void on_point1YSpinBox_valueChanged(double arg1);
    void on_point1ZSpinBox_valueChanged(double arg1);
    void on_point2XSpinBox_valueChanged(double arg1);
    void on_point2YSpinBox_valueChanged(double arg1);
    void on_point2ZSpinBox_valueChanged(double arg1);

    void onDistanceChanged(double dist);
    void onWorldPoint1Changed(double* pos);
    void onWorldPoint2Changed(double* pos);

private:
    void hookWidget(const vtkSmartPointer<vtkDistanceWidget>& widget);

private:
    Ui::DistanceWidgetConfig* m_configUi = nullptr;

    vtkSmartPointer<vtkDistanceWidget> m_2dWidget;
    vtkSmartPointer<vtkDistanceWidget> m_3dWidget;
    vtkSmartPointer<vtkDistanceRepresentation2D> m_2dRep;
    vtkSmartPointer<vtkDistanceRepresentation3D> m_3dRep;
};

#endif // DISTANCEWIDGETWINDOW_H
