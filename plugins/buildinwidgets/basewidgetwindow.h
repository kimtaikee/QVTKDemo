#ifndef BASEWIDGETWINDOW_H
#define BASEWIDGETWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class BaseWidgetWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkAbstractWidget;
class BaseWidgetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWidgetWindow(QWidget *parent = nullptr);
    ~BaseWidgetWindow();

    virtual void createWidget() = 0;

protected:
    template <class T>
    void setupConfigWidget(T* ui)
    {
        QWidget* configWidget = new QWidget(this);
        ui->setupUi(configWidget);
        m_ui->setupUi(this);
        m_ui->configLayout->addWidget(configWidget);
    }

    virtual void createUi();

protected:
    Ui::BaseWidgetWindow* m_ui = nullptr;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkActor> m_theActor;
};

#endif // BASEWIDGETWINDOW_H
