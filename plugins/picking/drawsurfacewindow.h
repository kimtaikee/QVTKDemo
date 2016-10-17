#ifndef DRAWSURFACEWINDOW_H
#define DRAWSURFACEWINDOW_H

#include <QWidget>

#include <vtkSmartPointer.h>

#include <utils/colorseries.h>

namespace Ui
{
    class DrawSurfaceWindow;
}

namespace Utils
{
    class TableModel;
}

namespace VtkUtils
{
    class VtkWidget;
}

class vtkPoints;
class vtkActor;
class vtkTextActor;
class MouseEventInteractorStyle;
class DrawSurfaceWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DrawSurfaceWindow(QWidget *parent = 0);
    ~DrawSurfaceWindow();

private slots:
    void on_resetButton_clicked();
    void addPoint(double* pos);
    void onCtrlPressed(bool pressed);
    void onMousePressed(double* pos);
    void on_exportButton_clicked();

private:
    void removePointActors();

private:
    MouseEventInteractorStyle* m_interactorStyle = nullptr;
    Ui::DrawSurfaceWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
    Utils::ColorSeries m_colorSeries;
    QList<vtkActor*> m_pointActors;
    vtkSmartPointer<vtkPoints> m_points;
    vtkSmartPointer<vtkActor> m_surfaceActor;
    vtkSmartPointer<vtkTextActor> m_promptActor;
    bool m_ctrlPressed = false;
};

#endif // DRAWSURFACEWINDOW_H
