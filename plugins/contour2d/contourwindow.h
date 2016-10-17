#ifndef CONTOURWINDOW_H
#define CONTOURWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class ContourWindow;
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
class vtkLineWidget;
class vtkPolyData;
class vtkScalarBarActor;
class vtkCubeAxesActor2D;
class vtkActor2D;
class vtkDelaunay2D;
class vtkCleanPolyData;
class vtkPoints;
class vtkDoubleArray;
class vtkTextActor;
class vtkLabeledDataMapper;
class vtkContextView;
class vtkChartXY;
class vtkPlot;
class ContourWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ContourWindow(QWidget *parent = 0);
    ~ContourWindow();

protected:
    enum { DefaultRows = 1000 };

    virtual void createContour();

private slots:
    void on_numOfContoursSpinBox_valueChanged(int arg1);
    void on_demoSourcesCombo_currentIndexChanged(int index);
    void on_labelGroupBox_toggled(bool arg1);
    void on_rowsSpinBox_valueChanged(int arg1);
    void on_plotScalarsCheckBox_toggled(bool checked);
    void on_randomButton_clicked();
    void on_bandedContourCheckBox_toggled(bool checked);
    void on_showContourLineCheckBox_toggled(bool checked);
    void on_pickPointCheckBox_toggled(bool checked);
    void on_importFileButton_clicked();
    void on_numOfLabelsSpinBox_valueChanged(int arg1);
    void on_labelFormatEdit_textChanged(const QString &arg1);

    void onModelToPointsConverterFinished();
    void onColorChanged(const QColor& clr);
    void onPointsToModelConverterFinished();
    void onPointReaderFinished();
    void onPointsToPolyDataConverterFinished();
    void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void onMousePressed(double* pos);
    void onLineWidgetPointsChanged(double* pot1, double* pot2);
    void onContourGeneratorFinished();
    void onGradientComboBoxActivated(int index);

protected:
    void init();
    void fireupPointsReader(const QString& file);
    void fireupModelToPointsConverter();
    void fireupSmoothContourGenerator();
    void fireupBandedContourGenerator();
    void showContourLabels(bool show);

    void setScalars(double min, double max);
    double scalarMin() const;
    double scalarMax() const;

    void createSmoothContour();
    void createBandedContour();
    void createSmoothContourLabels();
    void createBandedContourLabels();

    void setTriggerWidgetsEnabled(bool enabled);

protected:
    Ui::ContourWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    VtkUtils::VtkWidget* m_plotWidget = nullptr;
    vtkSmartPointer<vtkActor> m_contourActor;
    vtkSmartPointer<vtkActor> m_contourLineActor;
    vtkSmartPointer<vtkActor2D> m_labelActor;
    vtkSmartPointer<vtkPolyData> m_polyData;
    vtkSmartPointer<vtkScalarBarActor> m_scalarBar;
    vtkSmartPointer<vtkCubeAxesActor2D> m_cubeAxes;
    vtkSmartPointer<vtkDelaunay2D> m_planeDel;
    vtkSmartPointer<vtkCleanPolyData> m_cleanPolyData;
    vtkSmartPointer<vtkPoints> m_labelPoints;
    vtkSmartPointer<vtkDoubleArray> m_labelScalars;
    vtkSmartPointer<vtkTextActor> m_linePointsActor;
    vtkSmartPointer<vtkLabeledDataMapper> m_labeledDataMapper;
    vtkSmartPointer<vtkContextView> m_contextView;
    vtkSmartPointer<vtkChartXY> m_chart;
    vtkSmartPointer<vtkPlot> m_linePlot;

    Utils::TableModel* m_tableModel = nullptr;
    QColor m_labelColor = Qt::white;
    QString m_labelFormat = "%6.2g";
    vtkSmartPointer<vtkLineWidget> m_lineWidget;
    QColor m_lutColor1 = QColor(0,153,247);
    QColor m_lutColor2 = QColor(241,23,18);
    bool m_isBandedContour = false;
    bool m_isContourLineVisible = true;
    bool m_isPickPoint = false;
    bool m_isLabelVisible = false;
    int m_numberOfContours = 10;
    int m_numberOfLabels = 5;
    double m_scalar[2];
};

#endif // CONTOURWINDOW_H
