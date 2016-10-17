#ifndef CHART3DWINDOW_H
#define CHART3DWINDOW_H

#include <QWidget>

#include <utils/point3f.h>
#include <vtkSmartPointer.h>

namespace Ui
{
    class Chart3DWindow;
}

namespace Utils
{
    class TableModel;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkScalarBarActor;
class vtkCubeAxesActor2D;
class vtkPolyData;
class vtkOrientationMarkerWidget;
class Chart3DWindow : public QWidget
{
    Q_OBJECT

public:
    enum { DefaultRows = 100 };
    enum { DefaultLabels = 10 };

    explicit Chart3DWindow(QWidget *parent = 0);
    virtual ~Chart3DWindow();

protected:
    virtual void createChart(vtkPolyData* polyData);
    virtual void updateChart();

    Utils::TableModel* tableModel() const;
    vtkPolyData* polyData() const;

    qreal xRange() const;
    qreal yRange() const;
    qreal zRange() const;

private slots:
    void on_randomButton_clicked();
    void on_rowsSpinBox_valueChanged(int arg1);
    void on_coloringStyleCombo_currentIndexChanged(int index);
    void onModelToPointsConverterFinished();
    void onPointsToPolyDataConverterFinished();
    void onColorChanged(const QColor& clr);
    void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void on_showOutlineCheckBox_toggled(bool checked);
    void on_numOfLabelsSpinBox_valueChanged(int arg1);
    void on_showGridLinesCheckBox_toggled(bool toggled);
    void on_showPlanesCheckBox_toggled(bool toggled);
    void on_gridLinesColorButton_colorChanged(const QColor& clr);
    void on_planesColorButton_colorChanged(const QColor& clr);
    void on_importFileButton_clicked();
    void on_exportFileButton_clicked();
    void on_settingsGroupBox_toggled(bool toggled);

protected:
    void init();
    virtual void createUi();
    void showOrientationMarker(bool show = true);
    void showAxisPlanes(bool show = true);
    void showAxisGrid(bool show = true);
    void setAxisGridColor(const QColor& clr);
    void fireupModelToPointsConverter();
    void applyCustomColor();
    void applyScalarColor();

    template <class T>
    void setupConfigWidget(T* ui)
    {
        if (!m_configWidget)
            m_configWidget = new QWidget(this);
        ui->setupUi(m_configWidget);
        m_ui->settingsLayout->addWidget(m_configWidget);
    }

protected:
    vtkSmartPointer<vtkPolyData> m_polyData;
    Ui::Chart3DWindow *m_ui;
    Utils::TableModel* m_tableModel = nullptr;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_chartActor;
    vtkSmartPointer<vtkActor> m_axisPlanesActor;
    vtkSmartPointer<vtkActor> m_axisGridActor;
    vtkSmartPointer<vtkActor> m_outlineActor;
    vtkSmartPointer<vtkCubeAxesActor2D> m_cubeAxes;
    vtkSmartPointer<vtkScalarBarActor> m_scalarBar;
    bool m_useScalarColor = true;
    QColor m_customColor = Qt::blue;
    QColor m_axisGridColor = Qt::black;
    QColor m_axisPlaneColor = Qt::white;
    QList<Utils::Point3F> m_points;
    double m_bounds[6];
    QWidget* m_configWidget = nullptr;
};

#endif // CHART3DWINDOW_H
