#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>
#include <QMap>
#include <QDebug>

namespace Utils
{
    class TableModel;
}

namespace VtkUtils
{
    class VtkPlot3DWidget;
}

class vtkTable;
class vtkPlot3D;
class QTableView;
class PlotWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWindow(QWidget *parent = 0);
    virtual ~PlotWindow();

    static Utils::TableModel* model();

protected slots:
    virtual void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    virtual void onModelToVtkTableConverterFinished();
    virtual void onRowsChanged(int oldRows, int newRows);
    virtual void onColsChanged(int oldCols, int newCols);

    void onPointsReaderFinished();
    void onPointsToModelConverterFinished();
    void fireupModelToVtkTableConverter();

protected:
    void delayFire(int ms = 1000);
    void configTableView(QTableView* table);
    void importData();
    vtkTable* convertModelToVtkTable();
    virtual void fileImported(const QString& file);

    static Utils::TableModel* sModel;
    VtkUtils::VtkPlot3DWidget* m_plotWidget;
    QMap<int, vtkPlot3D*> m_columnPlotMap;
    vtkPlot3D* m_plot3d = nullptr;
};

#endif // PLOTWINDOW_H
