#include "plotwidget.h"

#include <utils/tablemodel.h>

#include <vtkutils/modeltovtktableconverter.h>

#include <vtkTable.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

#include <QTableView>
#include <QThreadPool>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>

static const int Cols = 7;
static const int Rows = 10;

Utils::TableModel* PlotWidget::sModel = nullptr;
PlotWidget::PlotWidget(QWidget *parent) : QWidget(parent)
{
    initConns();
}

PlotWidget::~PlotWidget()
{

}

Utils::TableModel* PlotWidget::model()
{
    if (!sModel) {
        sModel = new Utils::TableModel(Cols, Rows);
        sModel->setHorizontalHeaderData(QVariantList() << "X" << "Y1" << "Y2" << "Y3" << "Y4" << "Y5" << "Y6");
        sModel->random();

        for (int row = 0; row < Rows; ++row)
            sModel->setData(row, 0, row);
    }

    return sModel;
}

void PlotWidget::random()
{
    sModel->random();

    // auto-increment first column
    int rows = sModel->rowCount();
    for (int row = 0; row < rows; ++row)
        sModel->setData(row, 0, row);
}

void PlotWidget::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    // NO IMPL
}

void PlotWidget::onModelToVtkTableConverterFinished()
{
    VtkUtils::ModelToVtkTableConverter* converter = qobject_cast<VtkUtils::ModelToVtkTableConverter*>(sender());
    converter->deleteLater();
}

void PlotWidget::initConns()
{
    connect(model(), SIGNAL(rowsChanged(int,int)), this, SLOT(onModelRowsChanged(int,int)));
}

void PlotWidget::fireupModelToVtkTableConverter()
{
    VtkUtils::ModelToVtkTableConverter* converter = new VtkUtils::ModelToVtkTableConverter(sModel);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onModelToVtkTableConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

void PlotWidget::configTableView(QTableView *table)
{
    if (!table)
        return;

    table->setAlternatingRowColors(true);
}

vtkTable* PlotWidget::convertModelToVtkTable()
{
    vtkTable* table = vtkTable::New();

    int rows = sModel->rowCount();
    int cols = sModel->columnCount();

    for (int i = 0; i < rows; ++i) {
        vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::New();
        table->AddColumn(arr);
    }
    table->SetNumberOfRows(rows);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            qreal val = sModel->data(r, c);
            table->SetValue(r, c, val);
        }
    }

    return table;
}

void PlotWidget::setRows(int rows)
{
    sModel->resize(sModel->columnCount(), rows);
    sModel->blockSignals(true);
    sModel->random();
    sModel->blockSignals(false);
    emit sModel->layoutChanged();
    fireupModelToVtkTableConverter();
}

void PlotWidget::hookCheckBox(QCheckBox* cb)
{
    // prevent multiple connetions
    disconnect(cb, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged(int)));
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged(int)));
}

void PlotWidget::hookSpinBox(QSpinBox* sb)
{
    // prevent multiple connetions
    disconnect(sb, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChanged(int)));
    connect(sb, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChanged(int)));
}

void PlotWidget::hookDoubleSpinBox(QDoubleSpinBox* dsb)
{
    // prevent multiple connetions
    disconnect(dsb, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBoxValueChanged(double)));
    connect(dsb, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSpinBoxValueChanged(double)));
}

void PlotWidget::hookLineEdit(QLineEdit* le)
{
    // prevent multiple connetions
    disconnect(le, SIGNAL(textChanged(QString)), this, SLOT(onLineEditTextChanged(QString)));
    connect(le, SIGNAL(textChanged(QString)), this, SLOT(onLineEditTextChanged(QString)));
}

void PlotWidget::hookComboBox(QComboBox* cb)
{
    // prevent multiple connetions
    disconnect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
}




