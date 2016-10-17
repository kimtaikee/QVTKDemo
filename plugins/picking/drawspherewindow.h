#ifndef DRAWSPHEREWINDOW_H
#define DRAWSPHEREWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <utils/colorseries.h>

namespace Ui
{
    class DrawSphereWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkActor;
class vtkTextActor;
class MouseEventInteractorStyle;
class DrawSphereWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DrawSphereWindow(QWidget *parent = 0);
    ~DrawSphereWindow();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent* event);

private slots:
    void onMousePressed(double* pos);
    void onMouseMoved(double* pos);
    void onMouseReleased(double* pos);

private:
    Utils::ColorSeries m_colorSeries;
    Ui::DrawSphereWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    vtkSmartPointer<MouseEventInteractorStyle> m_interactorStyle;
    bool m_mousePressed = false;

    QColor m_currentSphereColor;
    vtkSmartPointer<vtkActor> m_currentActor;
    QList<vtkActor*> m_actorList;
    vtkSmartPointer<vtkSphereSource> m_sphereSource;
    vtkSmartPointer<vtkTextActor> m_promptActor;

    double m_pressedPos[3];
    double m_movedPos[3];
};

#endif // DRAWSPHEREWINDOW_H
