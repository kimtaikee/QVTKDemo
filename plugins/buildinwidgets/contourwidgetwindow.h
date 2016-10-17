#ifndef CONTOURWIDGETWINDOW_H
#define CONTOURWIDGETWINDOW_H

#include "basewidgetwindow.h"

namespace Ui
{
    class ContourWidgetConfig;
}

class vtkContourWidget;
class vtkContextActor;
class vtkPolyData;
class ContourWidgetWindow : public BaseWidgetWindow
{
    Q_OBJECT
public:
    explicit ContourWidgetWindow(QWidget* parent = nullptr);

protected:
    void createWidget();
    void createUi();

private slots:
    void on_clearButton_clicked();
    void on_showNodesCheckBox_toggled(bool checked);
    void onDataChanged(vtkPolyData* pd);

private:
    Ui::ContourWidgetConfig* m_configUi = nullptr;

    vtkSmartPointer<vtkContourWidget> m_contour;
};

#endif // CONTOURWIDGETWINDOW_H
