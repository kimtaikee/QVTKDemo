#ifndef SCALEAXESWINDOW_H
#define SCALEAXESWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class ScaleAxesWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

namespace Utils
{
    class TableModel;
}

class vtkActor;
class vtkPolyData;
class vtkDelaunay2D;
class vtkScalarBarActor;
class vtkCubeAxesActor2D;
class ScaleAxesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ScaleAxesWindow(QWidget *parent = 0);
    ~ScaleAxesWindow();

private slots:
    void onModelToPointsConverterFinished();
    void onPointsToPolyDataConverterFinished();
    void on_randomButton_clicked();
    void on_rowsSpinBox_valueChanged(int arg1);
    void on_xScaleSpinBox_valueChanged(double arg1);
    void on_yScaleSpinBox_valueChanged(double arg1);
    void on_zScaleSpinBox_valueChanged(double arg1);
    void on_scalingCheckBox_toggled(bool checked);
    void on_showActualBoundsCheckBox_toggled(bool checked);
    void on_useRangesCheckBox_toggled(bool checked);

    void on_useDataRangeCheckBox_toggled(bool checked);

private:
    void fireupModelToPointsConverter();
    void renderSurface(vtkPolyData* data);
    void printBounds();
    void applyScales();

private:
    Ui::ScaleAxesWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    Utils::TableModel* m_tableModel = nullptr;
    vtkSmartPointer<vtkActor> m_surfaceActor;
    vtkSmartPointer<vtkActor> m_outlineActor;
    vtkSmartPointer<vtkDelaunay2D> m_del;
    vtkSmartPointer<vtkCubeAxesActor2D> m_cubeAxes;
    vtkSmartPointer<vtkPolyData> m_polyData;
    double m_bounds[6];
};

#endif // SCALEAXESWINDOW_H
