#ifndef CONTOURWINDOW_H
#define CONTOURWINDOW_H

#include <QWidget>

namespace Ui
{
    class ContourWindow;
}

namespace Utils
{
    class TableModel;
}

namespace VtkUtils
{
    class Contour;
}

class ContourWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ContourWindow(QWidget *parent = 0);
    ~ContourWindow();

private slots:
    void onModelToVectorsConverterFinished();
    void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void onPointsReaderFinished();
    void onPointsToModelConverterFinished();
    void on_importDataButton_clicked();
    void on_pushButton_clicked();
    void on_rowsSpinbox_valueChanged(int arg1);
    void fireupModelToVectorsConverter();

private:
    Ui::ContourWindow *m_ui;
    Utils::TableModel* m_model = nullptr;
    VtkUtils::Contour* m_contour = nullptr;
};

#endif // CONTOURWINDOW_H
