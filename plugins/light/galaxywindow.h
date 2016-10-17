#ifndef GALAXYWINDOW_H
#define GALAXYWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

namespace Ui
{
    class GalaxyWindow;
}

namespace VtkUtils
{
    class VtkWidget;
    class MoveActorInteractorStyle;
}

class vtkLight;
class vtkActor;
class vtkLightActor;
class GalaxyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GalaxyWindow(QWidget *parent = 0);
    ~GalaxyWindow();

private slots:
    void onColorChanged(const QColor& clr);
    void onActorMoved(vtkActor* actor);
    void on_typeCombo_currentIndexChanged(int index);
    void on_groupBox_3_toggled(bool arg1);
    void on_coneAngleSpinBox_valueChanged(double arg1);

private:
    void init();
    void setActorTexture(vtkActor* actor, double radius, const QString& textureFile);

private:
    Ui::GalaxyWindow* m_ui;
    VtkUtils::VtkWidget* m_vtkWidget;
    vtkSmartPointer<VtkUtils::MoveActorInteractorStyle> m_interactorStyle;
    vtkSmartPointer<vtkLight> m_light;
    vtkSmartPointer<vtkLightActor> m_lightActor;
    vtkSmartPointer<vtkActor> m_earthActor;
    vtkSmartPointer<vtkActor> m_sunActor;
};

#endif // GALAXYWINDOW_H
