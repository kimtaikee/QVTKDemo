#ifndef SCREENSHOTWINDOW_H
#define SCREENSHOTWINDOW_H

#include <QWidget>

namespace Ui {
class ScreenshotWindow;
}

namespace VtkUtils
{
    class VtkWidget;
}

class ScreenshotWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenshotWindow(QWidget *parent = 0);
    ~ScreenshotWindow();

private slots:
    void on_exportButton_clicked();
    void on_browseButton_clicked();

private:
    Ui::ScreenshotWindow *m_ui;
    VtkUtils::VtkWidget* m_vtkWidget = nullptr;
};

#endif // SCREENSHOTWINDOW_H
