#ifndef SELECTAREAWINDOW_H
#define SELECTAREAWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class SelectAreaWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkPlanes;
class SelectAreaInteractorStyle;
class SelectAreaWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SelectAreaWindow(QWidget *parent = 0);
    ~SelectAreaWindow();

private slots:
    void onAreaSelected(vtkPlanes* plane);

private:
    Ui::SelectAreaWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<SelectAreaInteractorStyle> m_interactorStyle;
    vtkSmartPointer<vtkActor> m_selectionActor;
    vtkSmartPointer<vtkActor> m_modelActor;
};

#endif // SELECTAREAWINDOW_H
