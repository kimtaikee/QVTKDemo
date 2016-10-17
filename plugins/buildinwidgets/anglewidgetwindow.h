#ifndef ANGLEWIDGETWINDOW_H
#define ANGLEWIDGETWINDOW_H

#include "basewidgetwindow.h"

namespace Ui
{
    class AngleWidgetConfig;
}

class vtkAngleWidget;
class vtkAngleRepresentation2D;
class vtkAngleRepresentation3D;
class AngleWidgetWindow : public BaseWidgetWindow
{
    Q_OBJECT
public:
    explicit AngleWidgetWindow(QWidget* parent = nullptr);

protected:
    void createWidget();
    void createUi();

private slots:
    void on_typeCombo_currentIndexChanged(int index);

private:
    Ui::AngleWidgetConfig* m_configUi = nullptr;
    vtkSmartPointer<vtkAngleWidget> m_2dWidget;
    vtkSmartPointer<vtkAngleWidget> m_3dWidget;
    vtkSmartPointer<vtkAngleRepresentation2D> m_2dRep;
    vtkSmartPointer<vtkAngleRepresentation3D> m_3dRep;
};

#endif // ANGLEWIDGETWINDOW_H
