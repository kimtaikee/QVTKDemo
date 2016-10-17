#ifndef USECUSTOMWIDGETWINDOW_H
#define USECUSTOMWIDGETWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class UseCustomWidgetWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class CustomXYPlotWidget;
class CustomXYPlotRepresentation;
class UseCustomWidgetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UseCustomWidgetWindow(QWidget *parent = 0);
    ~UseCustomWidgetWindow();

private:
    Ui::UseCustomWidgetWindow *m_ui = nullptr;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;

    vtkSmartPointer<CustomXYPlotWidget> m_widget;
    vtkSmartPointer<CustomXYPlotRepresentation> m_rep;
};

#endif // USECUSTOMWIDGETWINDOW_H
