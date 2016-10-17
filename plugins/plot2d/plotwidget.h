#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QMap>

#include <vtkSmartPointer.h>

namespace Utils
{
    class TableModel;
}

namespace VtkUtils
{
    class VtkPlot2DWidget;
    class VtkPlotWidget;
}

class vtkTable;
class vtkPlot;
class QTableView;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QLineEdit;
class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = 0);
    virtual ~PlotWidget();

    static Utils::TableModel* model();
    static void random();

protected slots:
    virtual void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    virtual void onModelToVtkTableConverterFinished();

    virtual void onCheckBoxStateChanged(int state) { Q_UNUSED(state) }
    virtual void onSpinBoxValueChanged(int value) { Q_UNUSED(value) }
    virtual void onDoubleSpinBoxValueChanged(double value) { Q_UNUSED(value) }
    virtual void onComboBoxIndexChanged(int index) { Q_UNUSED(index) }
    virtual void onLineEditTextChanged(const QString& text) { Q_UNUSED(text) }
    virtual void onModelRowsChanged(int oldRows, int newRows) { Q_UNUSED(oldRows) Q_UNUSED(newRows) }

protected:
    virtual void initConns();

    void fireupModelToVtkTableConverter();
    void configTableView(QTableView* table);
    vtkTable* convertModelToVtkTable();
    void setRows(int rows);

    void hookCheckBox(QCheckBox* cb);
    void hookSpinBox(QSpinBox* sb);
    void hookDoubleSpinBox(QDoubleSpinBox* dsb);
    void hookLineEdit(QLineEdit* le);
    void hookComboBox(QComboBox* cb);

    template <class T>
    QList<T*> plots() const
    {
        QList<T*> plots_;
        QList<vtkPlot*> plotList = m_columnPlotMap.values();
        foreach (vtkPlot* plot, plotList)
            plots_.append(T::SafeDownCast(plot));
        return plots_;
    }

    static Utils::TableModel* sModel;
    VtkUtils::VtkPlotWidget* m_plotWidget;
    QMap<int, vtkPlot*> m_columnPlotMap;

};

#endif // PLOTWIDGET_H
