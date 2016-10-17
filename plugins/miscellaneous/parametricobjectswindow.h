#ifndef PARAMETRICOBJECTSWINDOW_H
#define PARAMETRICOBJECTSWINDOW_H

#include <QWidget>
#include <QMap>

#include <vtkSmartPointer.h>

namespace Ui
{
    class ParametricObjectsWindow;
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
class vtkPoints;
class ParametricObjectsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ParametricObjectsWindow(QWidget *parent = 0);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_romanRadiusSpinBox_valueChanged(double value);

    void on_ellipsoidXRadiusSpinBox_valueChanged(double value);
    void on_ellipsoidYRadiusSpinBox_valueChanged(double value);
    void on_ellipsoidZRadiusSpinBox_valueChanged(double value);

    void on_diniASpinBox_valueChanged(double value);
    void on_diniBSpinBox_valueChanged(double value);

    void on_conicSpiralASpinBox_valueChanged(double value);
    void on_conicSpiralBSpinBox_valueChanged(double value);
    void on_conicSpiralCSpinBox_valueChanged(double value);
    void on_conicSpiralNSpinBox_valueChanged(double value);

    // random hills
    void on_numOfHills_valueChanged(int value);
    void on_xVariance_valueChanged(double value);
    void on_yVariance_valueChanged(double value);
    void on_amplitude_valueChanged(double value);
    void on_randomSeed_valueChanged(int value);
    void on_randomGen_toggled(bool toggled);
    void on_xVarScaleFactor_valueChanged(double value);
    void on_yVarScaleFactor_valueChanged(double value);
    void on_ampScaleFactor_valueChanged(double value);

    // spline
    void on_dataTable_dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void on_rowsSpinBox_valueChanged(int value);
    void on_randomButton_clicked();
    void on_closedCheckBox_toggled(bool toggle);

    // superellipsoid
    void on_xRadiusSpinBox_valueChanged(double value);
    void on_yRadiusSpinBox_valueChanged(double value);
    void on_zRadiusSpinBox_valueChanged(double value);
    void on_n1SpinBox_valueChanged(double value);
    void on_n2SpinBox_valueChanged(double value);

    // supertoroid
    void on_xRadiusSpinBox_valueChanged_superToroid(double value);
    void on_yRadiusSpinBox_valueChanged_superToroid(double value);
    void on_zRadiusSpinBox_valueChanged_superToroid(double value);
    void on_n1SpinBox_valueChanged_superToroid(double value);
    void on_n2SpinBox_valueChanged_superToroid(double value);
    void on_ringRadiusSpinBox_valueChanged(double value);
    void on_crossSectionRadiusSpinBox_valueChanged(double value);

    void on_ringRadiusSpinBox_valueChanged_torus(double value);
    void on_crossSectionRadiusSpinBox_valueChanged_torus(double value);

private:
    void createConfigWidget(int func);

    template <class UiClass>
    QWidget* createConfigWidgetTempl();

    template <class UiClass>
    void createConfigWidgetConns(UiClass* ui);

    vtkSmartPointer<vtkPoints> pointsFromTable();

private:
    Ui::ParametricObjectsWindow* m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_objectActor;
    QMap<int, QWidget*> m_funcWidgetMap;
    QWidget* m_dummyWidget = nullptr;
    Utils::TableModel* m_tableModel = nullptr;

    double m_romanRadius = 1.0;

    double m_ellipsoidXRadius = 1;
    double m_ellipsoidYRadius = 1;
    double m_ellipsoidZRadius = 1;

    double m_diniA = 1.0;
    double m_diniB = 0.2;

    double m_conicSpiralA = 0.2;
    double m_conicSpiralB = 1;
    double m_conicSpiralC = 0.1;
    double m_conicSpiralN = 2;

    int m_randomHillsNumOfHills = 30;
    double m_randomHillsXVar = 2.5;
    double m_randomHillsYVar = 2.5;
    double m_randomHillsAmp = 2;
    int m_randomHillsRandomSeed = 1;
    bool m_randomHillsRandomGen = true;
    double m_randomHillsXVarScaleFactor = 1/3;
    double m_randomHillsYVarScaleFactor = 1/3;
    double m_randomHillsAmpScaleFactor = 1/3;

    bool m_splineClosed = false;

    double m_superEllipsoidXRadius = 1.0;
    double m_superEllipsoidYRadius = 1.0;
    double m_superEllipsoidZRadius = 1.0;
    double m_superEllipsoidN1 = 1.0;
    double m_superEllipsoidN2 = 1.0;

    double m_superToroidXRadius = 1.0;
    double m_superToroidYRadius = 1.0;
    double m_superToroidZRadius = 1.0;
    double m_superToroidN1 = 1.0;
    double m_superToroidN2 = 1.0;
    double m_superToroidRingRadius = 1.0;
    double m_superToroidCrossSectionRadius = .5;

    double m_torusRingRadius = 1.0;
    double m_torusCrossSectionRadius = .5;
};

#endif // PARAMETRICOBJECTSWINDOW_H
