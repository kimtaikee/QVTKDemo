#ifndef PIPELINEWINDOW_H
#define PIPELINEWINDOW_H

#include <QWidget>

namespace Ui
{
    class PipelineWindow;
}

class QMdiArea;
class PipelineWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PipelineWindow(QWidget *parent = 0);
    ~PipelineWindow();

private:
    Ui::PipelineWindow *m_ui;
    QMdiArea* m_mdiArea = nullptr;
};

#endif // PIPELINEWINDOW_H
