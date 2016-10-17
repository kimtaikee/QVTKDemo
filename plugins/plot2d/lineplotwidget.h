#ifndef LINEPLOTWIDGET_H
#define LINEPLOTWIDGET_H

#include "plotwidget.h"

class LinePlotSettings;
namespace Ui
{
    class TablePlotWidget;
    class LinePlotSettings;
}

class vtkPlot;
class vtkPlotLine;
class LinePlotWidget : public PlotWidget
{
    Q_OBJECT

public:
    explicit LinePlotWidget(QWidget *parent = 0);
    ~LinePlotWidget();

protected slots:
    void onModelToVtkTableConverterFinished();
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onCheckBoxStateChanged(int state);
    void onDoubleSpinBoxValueChanged(double value);
    void onComboBoxIndexChanged(int index);
    void onModelRowsChanged(int oldRows, int newRows);

protected:
    void initConns();

private slots:
    void on_pushButton_clicked();
    void on_rowsSpinBox_valueChanged(int arg1);

private:
    Ui::TablePlotWidget *m_ui;
    Ui::LinePlotSettings* m_settingsUi;
};

#endif // LINEPLOTWIDGET_H
