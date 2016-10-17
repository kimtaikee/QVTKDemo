#ifndef VOLUMERENDERINGWINDOW_H
#define VOLUMERENDERINGWINDOW_H

#include <QWidget>

namespace Ui
{
    class VolumeRenderingWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class VolumeRenderingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeRenderingWindow(QWidget *parent = 0);
    ~VolumeRenderingWindow();

private slots:
    void on_openButton_clicked();

private:
    Ui::VolumeRenderingWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
};

#endif // VOLUMERENDERINGWINDOW_H
