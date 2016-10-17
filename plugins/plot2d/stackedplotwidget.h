#ifndef STACKEDPLOTWIDGET_H
#define STACKEDPLOTWIDGET_H

#include "plotwidget.h"

namespace Ui
{
    class TablePlotWidget;
}

class vtkPlotStacked;
class StackedPlotWidget : public PlotWidget
{
    Q_OBJECT

public:
    explicit StackedPlotWidget(QWidget *parent = 0);
    ~StackedPlotWidget();

protected slots:
    void onModelToVtkTableConverterFinished();
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onModelRowsChanged(int oldRows, int newRows);

private slots:
    void on_pushButton_clicked();
    void on_rowsSpinBox_valueChanged(int arg1);

private:
    Ui::TablePlotWidget *m_ui;
    vtkSmartPointer<vtkPlotStacked> m_stacked;
};

#endif // STACKEDPLOTWIDGET_H
