#ifndef ISOSURFACEWINDOW_H
#define ISOSURFACEWINDOW_H

#include "filterwindow.h"

namespace Ui
{
    class IsosurfaceConfig;
}

class IsosurfaceWindow : public FilterWindow
{
    Q_OBJECT

public:
    explicit IsosurfaceWindow(QWidget *parent = 0);
    ~IsosurfaceWindow();

    void apply();

protected:
    void modelReady();
    void createUi();
    void colorsChanged();

protected slots:
    void on_openFileButton_clicked();
    void onDoubleSpinBoxValueChanged(double value);
    void onSpinBoxValueChanged(int value);
    void onComboBoxIndexChanged(int index);
    void on_gradientCombo_activated(int index);

protected:
    Ui::IsosurfaceConfig* m_configUi = nullptr;
    double m_minScalar = .0;
    double m_maxScalar = .0;
    int m_numOfContours = 10;
    QString m_currentScalarName;
};

#endif // ISOSURFACEWINDOW_H
