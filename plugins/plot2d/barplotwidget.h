#ifndef BARPLOTWIDGET_H
#define BARPLOTWIDGET_H

#include "plotwidget.h"

namespace Ui
{
    class TablePlotWidget;
    class BarPlotSettings;
}

class BarPlotWidget : public PlotWidget
{
    Q_OBJECT

public:
    explicit BarPlotWidget(QWidget *parent = 0);
    ~BarPlotWidget();

protected slots:
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onModelToVtkTableConverterFinished();
    void onCheckBoxStateChanged(int state);
    void onDoubleSpinBoxValueChanged(double value);
    void onComboBoxIndexChanged(int index);
    void onModelRowsChanged(int oldRows, int newRows);

private slots:
    void on_pushButton_clicked();
    void on_rowsSpinBox_valueChanged(int arg1);

protected:
    void initConns();

private:
    Ui::TablePlotWidget *m_ui;
    Ui::BarPlotSettings* m_settingsUi;
};

#endif // BARPLOTWIDGET_H
