#ifndef PICKACTORWINDOW_H
#define PICKACTORWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>
#include <utils/colorseries.h>

namespace Ui
{
    class PickActorWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkTextActor;
class PickActorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PickActorWindow(QWidget *parent = 0);
    ~PickActorWindow();

private slots:
    void on_xMinSpinBox_valueChanged(double arg1);
    void on_xMaxSpinBox_valueChanged(double arg1);
    void on_yMinSpinBox_valueChanged(double arg1);
    void on_zMinSpinBox_valueChanged(double arg1);
    void on_yMaxSpinBox_valueChanged(double arg1);
    void on_zMaxSpinBox_valueChanged(double arg1);
    void on_addButton_clicked();
    void on_importButton_clicked();
    void on_exportButton_clicked();

private:
    void populate();
    void addActor(int sourceType);
    double randomXPos() const;
    double randomYPos() const;
    double randomZPos() const;

private:
    Ui::PickActorWindow* m_ui;
    Utils::ColorSeries m_colorSeries;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<vtkTextActor> m_promptActor;
    double m_bounds[6];
};

#endif // PICKACTORWINDOW_H
