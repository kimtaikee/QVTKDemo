#ifndef SMOOTHWINDOW_H
#define SMOOTHWINDOW_H

#include "filterwindow.h"

namespace Ui
{
    class SmoothConfig;
}

class SmoothWindow : public FilterWindow
{
    Q_OBJECT
public:
    explicit SmoothWindow(QWidget* parent = nullptr);

    void apply();

private slots:
    void on_openButton_clicked();
    void on_convergenceSpinBox_valueChanged(double arg1);
    void on_numOfIterationsSpinBox_valueChanged(int arg1);
    void on_featureEdgeSmoothingCheckBox_toggled(bool checked);
    void on_boundarySmoothingCheckBox_toggled(bool checked);
    void on_featureAngleSpinBox_valueChanged(double arg1);
    void on_edgeAngleSpinBox_valueChanged(double arg1);

protected:
    void createUi();

private:
    Ui::SmoothConfig* m_configUi = nullptr;

    int m_numberOfIterations;

    bool m_featureEdgeSmoothing;
    bool m_boundarySmoothing;

    double m_convergence;
    double m_featureAngle;
    double m_edgeAngle;
};

#endif // SMOOTHWINDOW_H
