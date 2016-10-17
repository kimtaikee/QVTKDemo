#ifndef POINTS3DWINDOW_H
#define POINTS3DWINDOW_H

#include "chart3dwindow.h"

#include <vtkSmartPointer.h>

namespace Ui
{
    class ConfigPoints3D;
}

class vtkActor2D;
class Points3DWindow : public Chart3DWindow
{
    Q_OBJECT
public:
    explicit Points3DWindow(QWidget* parent = nullptr);
    ~Points3DWindow();

protected:
    void createChart(vtkPolyData* polydata);
    void updateChart();
    void createUi();

private slots:
    void on_thetaResolutionSpinBox_valueChanged(int arg1);
    void on_phiResolutionSpinBox_valueChanged(int arg1);
    void on_scaleModeCombo_currentIndexChanged(int index);
    void on_scaleFactorSpinBox_valueChanged(double arg1);
    void on_scalingGroupBox_toggled(bool arg1);
    void on_labelsGroupBox_toggled(bool arg1);
    void onColorChanged(const QColor& clr);
    void showLabels(bool show = true);
    void on_labelsFormatEdit_textChanged(const QString &arg1);

protected:
    Ui::ConfigPoints3D* m_configUi = nullptr;
    int m_thetaResolution = 8;
    int m_phiResolution = 8;
    bool m_scaling = true;
    int m_scaleMode = 0; // VTK_SCALE_BY_SCALAR
    qreal m_scaleFactor = 1.0;
    vtkSmartPointer<vtkActor2D> m_labelsActor;
    QString m_labelsFormat = "%6.2f";
    QColor m_labelsColor = Qt::white;
};

#endif // POINTS3DWINDOW_H
