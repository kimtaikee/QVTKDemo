#ifndef PLOTLINE3DWINDOW_H
#define PLOTLINE3DWINDOW_H

#include "plotwindow.h"

namespace Ui
{
    class TableWidgetWindow;
}

class PlotLine3DWindow : public PlotWindow
{
    Q_OBJECT

public:
    explicit PlotLine3DWindow(QWidget *parent = 0);
    ~PlotLine3DWindow();

protected slots:
    void onModelToVtkTableConverterFinished();
    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onRowsChanged(int oldRows, int newRows);

protected:
    void fileImported(const QString &file);

private slots:
    void on_pushButton_clicked();
    void on_importButton_clicked();
    void on_rowsSpinBox_valueChanged(int arg1);

private:
    Ui::TableWidgetWindow *m_ui;
};

#endif // PLOTLINE3DWINDOW_H
