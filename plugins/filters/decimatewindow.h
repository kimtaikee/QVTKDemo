#ifndef DECIMATEWINDOW_H
#define DECIMATEWINDOW_H

#include "filterwindow.h"

namespace Ui
{
    class DecimateConfig;
}

class DecimateWindow : public FilterWindow
{
    Q_OBJECT
public:
    explicit DecimateWindow(QWidget* parent = nullptr);

    void apply();

private slots:
    void on_targetReductionSpinBox_valueChanged(double arg1);
    void on_preserveTopologyCheckBox_toggled(bool checked);
    void on_splittingCheckBox_toggled(bool checked);
    void on_presplitMeshCheckBox_toggled(bool checked);
    void on_accumulateErrorCheckBox_toggled(bool checked);
    void on_boundaryVertexDeletionCheckBox_toggled(bool checked);
    void on_featureAngleSpinBox_valueChanged(double arg1);
    void on_splitAngleSpinBox_valueChanged(double arg1);
    void on_outputPointsPrecisionSpinBox_valueChanged(int arg1);
    void on_inflectionPointRatioSpinBox_valueChanged(double arg1);
    void on_degreeSpinBox_valueChanged(int arg1);
    void on_openButton_clicked();

protected:
    void createUi();

private:
    Ui::DecimateConfig* m_configUi = nullptr;

    double m_targetReduction;
    double m_featureAngle;
    double m_splitAngle;
    double m_inflectionPointRatio;

    bool m_preserveTopology;
    bool m_splitting;
    bool m_presplitMesh;
    bool m_accumulateError;
    bool m_boundaryVertexDeletion;

    int m_outputPointsPrecision;
    int m_degree;
};

#endif // DECIMATEWINDOW_H
