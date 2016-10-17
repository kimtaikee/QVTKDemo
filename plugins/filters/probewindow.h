#ifndef PROBEWINDOW_H
#define PROBEWINDOW_H

#include "filterwindow.h"

namespace Ui
{
    class ProbeConfig;
}

class vtkLineWidget;
class vtkBoxWidget;
class vtkSphereWidget;
class vtkPlaneWidget;
class vtkImplicitPlaneWidget;
class QCustomPlot;
class ProbeWindow : public FilterWindow
{
    Q_OBJECT
public:
    explicit ProbeWindow(QWidget* parent = nullptr);

    void apply();

protected:
    void modelReady();
    void createUi();

private slots:
    void on_openButton_clicked();
    void on_sourceCombo_currentIndexChanged(int index);
    void onLineWidgetPointsChanged(double* point1, double* point2);
    void onSphereWidgetCenterChanged(double* center);
    void onImplicitPlaneWidgetOriginChanged(double* origin);
    void onImplicitPlaneWidgetNormalChanged(double* normal);
    void on_sphereRadius_valueChanged(double arg1);

private:
    void showProbeWidget();

protected:
    enum WidgetType { WT_Line, WT_Sphere, WT_Box, WT_ImplicitPlane};

    Ui::ProbeConfig* m_configUi = nullptr;

    WidgetType m_widgetType = WT_Line;
    vtkLineWidget* m_lineWidget = nullptr;
    vtkSphereWidget* m_sphereWidget = nullptr;
    vtkBoxWidget* m_boxWidget = nullptr;
    vtkImplicitPlaneWidget* m_implicitPlaneWidget = nullptr;
    QCustomPlot* m_plotWidget = nullptr;

    double m_linePoint1[3];
    double m_linePoint2[3];

    double m_sphereCenter[3];
    double m_sphereRadius;

    double m_planeOrigin[3];
    double m_planeNormal[3];
};

#endif // PROBEWINDOW_H
