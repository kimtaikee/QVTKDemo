#ifndef PIEPLOTWIDGET_H
#define PIEPLOTWIDGET_H

#include "plotwidget.h"

namespace Ui
{
    class TablePlotWidget;
}

class PiePlotWidget : public PlotWidget
{
    Q_OBJECT
public:
    explicit PiePlotWidget(QWidget* parent = nullptr);
    ~PiePlotWidget();

protected:
    void onModelRowsChanged(int oldRows, int newRows);

private:
    Ui::TablePlotWidget *m_ui;
};

#endif // PIEPLOTWIDGET_H
