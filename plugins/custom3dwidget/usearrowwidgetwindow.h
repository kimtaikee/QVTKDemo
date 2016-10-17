#ifndef USEARROWWIDGETWINDOW_H
#define USEARROWWIDGETWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class UseArrowWidgetWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkArrowWidget;
class UseArrowWidgetWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UseArrowWidgetWindow(QWidget *parent = 0);

private slots:
    void on_pot1xSpinBox_valueChanged(double arg1);
    void on_pot1ySpinBox_valueChanged(double arg1);
    void on_pot1zSpinBox_valueChanged(double arg1);
    void on_pot2xSpinBox_valueChanged(double arg1);
    void on_pot2ySpinBox_valueChanged(double arg1);
    void on_pot2zSpinBox_valueChanged(double arg1);

private:
    Ui::UseArrowWidgetWindow* m_ui = nullptr;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkArrowWidget> m_widget;
};

#endif // USEARROWWIDGETWINDOW_H
