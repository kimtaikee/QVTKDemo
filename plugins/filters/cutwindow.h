#ifndef CUTWINDOW_H
#define CUTWINDOW_H

#include "filterwindow.h"

#include <vtkSmartPointer.h>

namespace Ui
{
    class CutConfig;
}

class vtkBoxWidget;
class vtkSphereWidget;
class vtkImplicitPlaneWidget;
class vtk3DWidget;
class vtkPlanes;
class CutWindow : public FilterWindow
{
    Q_OBJECT
protected:
    enum CutType { Plane, Sphere, Box };

    explicit CutWindow(QWidget* parent = nullptr);

    void setNormal(double normal[3]);
    void setOrigin(double origin[3]);
    void setRadius(double radius);
    void updateCutWidget();

    void setCutType(CutType type);
    CutType cutType() const;

protected slots:
    void onOriginChanged(double* orgin);
    void onNormalChanged(double* normal);
    void onCenterChanged(double* center);
    void onPlanesChanged(vtkPlanes* planes);
    void showContourLines(bool show = true);

private slots:
    void on_cutTypeCombo_currentIndexChanged(int index);
    void on_displayEffectCombo_currentIndexChanged(int index);
    void on_rowsSpinBox_valueChanged(int arg1);
    void on_radiusSpinBox_valueChanged(double arg1);
    void on_originXSpinBox_valueChanged(double arg1);
    void on_originYSpinBox_valueChanged(double arg1);
    void on_originZSpinBox_valueChanged(double arg1);
    void on_normalXSpinBox_valueChanged(double arg1);
    void on_normalYSpinBox_valueChanged(double arg1);
    void on_normalZSpinBox_valueChanged(double arg1);
    void on_centerXSpinBox_valueChanged(double arg1);
    void on_centerYSpinBox_valueChanged(double arg1);
    void on_centerZSpinBox_valueChanged(double arg1);
    void on_openFileButton_clicked();
    void on_randomButton_clicked();
    void on_showPlaneCheckBox_toggled(bool checked);
    void on_showContourLinesCheckBox_toggled(bool checked);
    void on_gradientCombo_activated(int index);

protected:
    void modelReady();
    void createUi();
    void dataChanged();

    void safeOff(vtk3DWidget* widget);

    void resetPlaneWidget();
    void resetSphereWidget();
    void resetBoxWidget();

protected:
    double m_normal[3];
    double m_origin[3];
    double m_center[3];
    double m_radius;

    vtkSmartPointer<vtkBoxWidget> m_boxWidget;
    vtkSmartPointer<vtkSphereWidget> m_sphereWidget;
    vtkSmartPointer<vtkImplicitPlaneWidget> m_planeWidget;

    Ui::CutConfig* m_configUi = nullptr;
    CutType m_cutType = Plane;
    vtkSmartPointer<vtkPlanes> m_planes;
    vtkSmartPointer<vtkActor> m_contourLinesActor;
};

#endif // CUTWINDOW_H
