#ifndef SCATTERPLOTWIDGET_H
#define SCATTERPLOTWIDGET_H

#include "plotwidget.h"

namespace Ui
{
    class TablePlotWidget;
    class ScatterPlotSettings;
}

class ScatterPlotWidget : public PlotWidget
{
    Q_OBJECT

public:
    explicit ScatterPlotWidget(QWidget *parent = 0);
    ~ScatterPlotWidget();

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
    Ui::ScatterPlotSettings* m_settingsUi;
};

#endif // SCATTERPLOTWIDGET_H
