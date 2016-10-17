#ifndef BAGPLOTWIDGET_H
#define BAGPLOTWIDGET_H

#include "plotwidget.h"

namespace Ui
{
    class TablePlotWidget;
}

class vtkPlotBag;
class BagPlotWidget : public PlotWidget
{
    Q_OBJECT
public:
    explicit BagPlotWidget(QWidget* parent = nullptr);
    ~BagPlotWidget();

protected slots:
    void onModelToVtkTableConverterFinished();
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onModelRowsChanged(int oldRows, int newRows);

private slots:
    void on_pushButton_clicked();
    void on_rowsSpinBox_valueChanged(int arg1);

private:
    Ui::TablePlotWidget *m_ui;
    vtkSmartPointer<vtkPlotBag> m_plotBag;
};

#endif // BAGPLOTWIDGET_H
