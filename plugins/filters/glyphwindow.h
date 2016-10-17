#ifndef GLYPHWINDOW_H
#define GLYPHWINDOW_H

#include "filterwindow.h"

namespace Ui
{
    class GlyphConfig;
}

class vtkActor2D;
class GlyphWindow : public FilterWindow
{
    Q_OBJECT
public:
    explicit GlyphWindow(QWidget* parent = nullptr);

    void apply();

protected:
    void modelReady();
    void createUi();

private slots:
    void on_sizeSpinBox_valueChanged(double arg1);
    void on_shapeCombo_currentIndexChanged(int index);
    void onColorChanged(const QColor& clr);
    void on_openButton_clicked();
    void on_displayEffectCombo_currentIndexChanged(int index);
    void on_rowsSpinBox_valueChanged(int arg1);
    void on_labelGroupBox_toggled(bool arg1);
    void showLabels(bool show = true);
    void setLabelsColor(const QColor& clr);
    void on_modeCombo_currentIndexChanged(int index);
    void on_randomButton_clicked();

private:
    enum Shape { Arrow, Cone, Line, Cylinder, Sphere, Point};

    Ui::GlyphConfig* m_configUi = nullptr;
    QColor m_glyphColor = Qt::white;
    QColor m_labelColor = Qt::white;
    double m_size = 3;
    int m_labelMode = 1; // VTK_LABEL_SCALARS
    Shape m_shape = Arrow;
    vtkActor2D* m_labelActor = nullptr;
    bool m_labelVisible = false;
};

#endif // GLYPHWINDOW_H
