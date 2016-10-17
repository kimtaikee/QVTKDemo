#ifndef COLORMAPWINDOW_H
#define COLORMAPWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class ColorMapWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkScalarBarActor;
class ColorMapWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ColorMapWindow(QWidget *parent = 0);
    ~ColorMapWindow();

    void apply();

private slots:
    void on_hueMinSpinBox_valueChanged(double arg1);
    void on_hueMaxSpinBox_valueChanged(double arg1);
    void on_saturationMinSpinBox_valueChanged(double arg1);
    void on_saturationMaxSpinBox_valueChanged(double arg1);
    void on_valueMinSpinBox_valueChanged(double arg1);
    void on_valueMaxSpinBox_valueChanged(double arg1);
    void on_alphaMinSpinBox_valueChanged(double arg1);
    void on_alphaMaxSpinBox_valueChanged(double arg1);
    void on_numOfColorsSpinBox_valueChanged(int arg1);
    void on_gradientCombo_activated(int index);

private:
    Ui::ColorMapWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_modelActor;
    vtkSmartPointer<vtkScalarBarActor> m_scalarBar;

    double m_hueMin = 0.667;
    double m_hueMax = 1.0;
    double m_saturationMin = 1.0;
    double m_saturationMax = 1.0;
    double m_valueMin = 1.0;
    double m_valueMax = 1.0;
    double m_alphaMin = 1.0;
    double m_alphaMax = 1.0;
    int m_numOfColors = 12;
};

#endif // COLORMAPWINDOW_H
