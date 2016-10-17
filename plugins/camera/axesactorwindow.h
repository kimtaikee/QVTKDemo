#ifndef AXESACTORWINDOW_H
#define AXESACTORWINDOW_H

#include <QWidget>

namespace Ui
{
    class AxesActorWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class AxesActorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AxesActorWindow(QWidget *parent = 0);
    ~AxesActorWindow();

private:
    Ui::AxesActorWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
};

#endif // AXESACTORWINDOW_H
