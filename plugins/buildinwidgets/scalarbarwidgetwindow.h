#ifndef SCALARBARWIDGETWINDOW_H
#define SCALARBARWIDGETWINDOW_H

#include "basewidgetwindow.h"

namespace Ui
{
    class ScalarBarWidgetConfig;
}

class vtkScalarBarWidget;
class vtkScalarBarRepresentation;
class ScalarBarWidgetWindow : public BaseWidgetWindow
{
    Q_OBJECT
public:
    explicit ScalarBarWidgetWindow(QWidget* parent = nullptr);

protected:
    void createWidget();
    void createUi();

private slots:
    void on_orientCombo_currentIndexChanged(int index);
    void on_gradientCombo_activated(int index);
    void on_numOfLabelsSpinBox_valueChanged(int arg1);
    void on_numOfColorsSpinBox_valueChanged(int arg1);
    void on_selectableCheckBox_toggled(bool checked);
    void on_resizableCheckBox_toggled(bool checked);

private:
    Ui::ScalarBarWidgetConfig* m_configUi = nullptr;
    vtkSmartPointer<vtkScalarBarWidget> m_scalarBar;
    vtkSmartPointer<vtkScalarBarRepresentation> m_rep;
};

#endif // SCALARBARWIDGETWINDOW_H
