#ifndef PICKCELLWINDOW_H
#define PICKCELLWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class PickCellWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class PickCellInteractorStyle;
class vtkActor;
class PickCellWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PickCellWindow(QWidget *parent = 0);
    ~PickCellWindow();

private slots:
    void onCellPicked(int cellId);

private:
    Ui::PickCellWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_selectionActor;
    vtkSmartPointer<vtkActor> m_modelActor;
    vtkSmartPointer<PickCellInteractorStyle> m_interactorStyle;
};

#endif // PICKCELLWINDOW_H
