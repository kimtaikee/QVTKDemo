#ifndef STREAMLINEWINDOW_H
#define STREAMLINEWINDOW_H

#include "filterwindow.h"

#include <vtkStreamTracer.h>

namespace Ui
{
    class StreamlineConfig;
}

class vtkLineWidget;
class vtkPointWidget;
class StreamlineWindow : public FilterWindow
{
    Q_OBJECT
public:
    explicit StreamlineWindow(QWidget* parent = nullptr);
    ~StreamlineWindow();

    void apply();

protected:
    void modelReady();
    void createUi();

private slots:
    void on_sourceCombo_currentIndexChanged(int index);
    void on_openButton_clicked();
    void on_numOfPointsSpinBox_valueChanged(int arg1);
    void on_radiusSpinBox_valueChanged(double arg1);
    void on_centerXSpinBox_valueChanged(double arg1);
    void on_centerYSpinBox_valueChanged(double arg1);
    void on_centerZSpinBox_valueChanged(double arg1);
    void on_integratorTypeCombo_currentIndexChanged(int index);
    void on_integrationDirectionCombo_currentIndexChanged(int index);
    void on_point1XSpinBox_valueChanged(double arg1);
    void on_point1YSpinBox_valueChanged(double arg1);
    void on_point1ZSpinBox_valueChanged(double arg1);
    void on_point2XSpinBox_valueChanged(double arg1);
    void on_point2YSpinBox_valueChanged(double arg1);
    void on_point2ZSpinBox_valueChanged(double arg1);
    void on_displayModeCombo_currentIndexChanged(int index);
    void on_displayEffectCombo_currentIndexChanged(int index);
    void on_numOfSidesSpinBox_valueChanged(int arg1);
    void on_radiusFactorSpinBox_valueChanged(double arg1);
    void on_cappingCheckBox_toggled(bool checked);
    void on_distanceFactorSpinBox_valueChanged(int arg1);
    void on_onRatioSpinBox_valueChanged(int arg1);
    void on_offsetSpinBox_valueChanged(int arg1);
    void on_passLinesCheckBox_toggled(bool checked);
    void on_closeSurfaceCheckBox_toggled(bool checked);
    void on_orientLoopsCheckBox_toggled(bool checked);
    void on_ruledModeCombo_currentIndexChanged(int index);
    void on_resolutionSpinBox_valueChanged(int arg1);
    void onLinePointsChanged(double* point1, double* point2);
    void onPointPositionChanged(double* position);

private:
    void showConfigGroupBox(int index);
    void updatePointActor();
    void updateLineActor();

private:

    struct TubeParams
    {
        double radius;
        double radiusFactor = 10;
        int numberOfSides = 3; // minimum is 3
        int offset = 0;
        bool capping = false;
        bool useDefaultNormal = true;
    };

    struct RuledSurfaceParams
    {
        int distanceFactor;
        int onRatio;
        int offset;
        bool closeSurface;
        bool passLines;
        bool orientLoops;
        int ruledMode;
        int resolution[2];
    };

    enum SourceType { Point, Line };
    enum DisplayMode { None, Tube, Surface };
    Ui::StreamlineConfig* m_configUi = nullptr;

    SourceType m_source = Point;
    DisplayMode m_displayMode = None;
    double m_center[3];
    double m_point1[3];
    double m_point2[3];
    int m_numberOfPoints = 100;
    double m_radius = 1.0;
    int m_integratorType = vtkStreamTracer::RUNGE_KUTTA2;
    int m_integrationDirection = vtkStreamTracer::FORWARD;

    TubeParams m_tubeParams;
    RuledSurfaceParams m_ruledSurfaceParams;

    vtkLineWidget* m_lineWidget = nullptr;
    vtkPointWidget* m_pointWidget = nullptr;
};

#endif // STREAMLINEWINDOW_H
