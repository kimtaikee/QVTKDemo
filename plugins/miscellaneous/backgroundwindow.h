#ifndef BACKGROUNDWINDOW_H
#define BACKGROUNDWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class BackgroundWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkImageActor;
class vtkRenderer;
class vtkSuperquadricSource;
class BackgroundWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BackgroundWindow(QWidget *parent = 0);
    ~BackgroundWindow();

private slots:
    void on_styleCombo_currentIndexChanged(int index);
    void on_browseButton_clicked();
    void onColorChanged(const QColor& clr);
    void on_thetaResolutionSpinBox_valueChanged(int arg1);
    void on_phiResolutionSpinBox_valueChanged(int arg1);
    void on_thetaRoundnessSpinBox_valueChanged(double arg1);
    void on_phiRoundnessSpinBox_valueChanged(double arg1);
    void on_thicknessSpinBox_valueChanged(double arg1);
    void on_shapeComboBox_currentIndexChanged(int index);

private:
    Ui::BackgroundWindow *m_ui;
    vtkSmartPointer<vtkActor> m_modelActor;
    vtkSmartPointer<vtkImageActor> m_imageActor;
    vtkSmartPointer<vtkRenderer> m_bgRenderer;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkSuperquadricSource> m_superquadricSource;
};

#endif // BACKGROUNDWINDOW_H
