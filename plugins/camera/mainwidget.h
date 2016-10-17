#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class MainWidget;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkRenderer;
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    Ui::MainWidget * m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_actor;
    vtkSmartPointer<vtkRenderer> m_actorRenderer;
    vtkSmartPointer<vtkRenderer> m_topViewRenderer;
    vtkSmartPointer<vtkRenderer> m_bottomViewRenderer;
//    vtkSmartPointer<vtkRenderer> m_actorRenderer;
};

#endif // MAINWIDGET_H
