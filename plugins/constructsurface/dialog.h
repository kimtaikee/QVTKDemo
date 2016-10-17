#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui
{
    class Dialog;
}

namespace Utils
{
    class TableModel;
}

namespace VtkUtils
{
    class Surface;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButton_clicked();
    void onPointsReaderFinished();
    void onPointsToModelConverterFinished();
    void onModelToPointsConverterFinished();
    void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void on_pushButton_2_clicked();
    void on_rowsSpinbox_valueChanged(int arg1);
    void fireupModelToPointsConverter();

private:
    Ui::Dialog *m_ui;
    VtkUtils::Surface* m_vtkWidget = nullptr;
    Utils::TableModel* m_model = nullptr;
};

#endif // DIALOG_H
