#ifndef REPRESENTATIONWIDGET_H
#define REPRESENTATIONWIDGET_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class RepresentationWidget;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class RepresentationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RepresentationWidget(QWidget *parent = 0);
    ~RepresentationWidget();

private slots:
    void on_surfaceRadioButton_clicked(bool checked);
    void on_pointsRadioButton_clicked(bool checked);
    void on_wireframeRadioButton_clicked(bool checked);

private:
    Ui::RepresentationWidget *m_ui;
    vtkSmartPointer<vtkActor> m_actor;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
};

#endif // REPRESENTATIONWIDGET_H
