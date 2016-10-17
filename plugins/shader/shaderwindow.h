#ifndef SHADERWINDOW_H
#define SHADERWINDOW_H

#include <QWidget>

namespace Ui
{
    class ShaderWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class ShaderWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ShaderWindow(QWidget *parent = 0);
    ~ShaderWindow();

private:
    Ui::ShaderWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
};

#endif // SHADERWINDOW_H
