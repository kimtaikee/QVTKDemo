#include "plotwindow.h"

#include <utils/tablemodel.h>
#include <utils/pointsreader.h>
#include <utils/pointstomodelconverter.h>

#include <vtkutils/modeltovtktableconverter.h>

#include <vtkTable.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

#include <QTableView>
#include <QThreadPool>
#include <QTimer>
#include <QFileDialog>

Utils::TableModel* PlotWindow::sModel = nullptr;

static const int Rows = 10;
static const int Cols = 4; // x,y,z,colors
PlotWindow::PlotWindow(QWidget *parent) : QWidget(parent)
{

}

PlotWindow::~PlotWindow()
{

}

Utils::TableModel* PlotWindow::model()
{
    if (!sModel) {
         sModel = new Utils::TableModel(Cols, Rows);
         sModel->setHorizontalHeaderData(QVariantList() << "x" << "y" << "z" << "color");
         sModel->random();
    }

    return sModel;
}

void PlotWindow::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    // NO IMPL
}

void PlotWindow::onModelToVtkTableConverterFinished()
{
    VtkUtils::ModelToVtkTableConverter* converter = qobject_cast<VtkUtils::ModelToVtkTableConverter*>(sender());
    converter->deleteLater();
}

void PlotWindow::onRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Q_UNUSED(newRows)
}

void PlotWindow::onColsChanged(int oldCols, int newCols)
{
    Q_UNUSED(oldCols)
    Q_UNUSED(newCols)
}

void PlotWindow::onPointsReaderFinished()
{
    Utils::PointsReader* reader = qobject_cast<Utils::PointsReader*>(sender());
    QList<Utils::Point3F> points = reader->points();
    reader->deleteLater();

    if (points.isEmpty())
        return;

    Utils::PointsToModelConverter* converter = new Utils::PointsToModelConverter(points, sModel);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onPointsToModelConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

void PlotWindow::onPointsToModelConverterFinished()
{
    Utils::PointsToModelConverter* converter = qobject_cast<Utils::PointsToModelConverter*>(sender());
    emit sModel->layoutChanged();
    converter->deleteLater();
    fireupModelToVtkTableConverter();
}

void PlotWindow::fireupModelToVtkTableConverter()
{
    VtkUtils::ModelToVtkTableConverter* converter = new VtkUtils::ModelToVtkTableConverter(sModel);
    converter->setLabels(QStringList() << "X" << "Y" << "Z" << "Color");
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onModelToVtkTableConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

void PlotWindow::delayFire(int ms)
{
    sModel->blockSignals(true);
    sModel->random();
    sModel->blockSignals(false);
    emit sModel->layoutChanged();

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(ms);
    connect(&timer, SIGNAL(timeout()), this, SLOT(fireupModelToVtkTableConverter()));
    timer.start();
}

void PlotWindow::configTableView(QTableView *table)
{
    if (!table)
        return;

    table->setAlternatingRowColors(true);
}

void PlotWindow::importData()
{
    QString file = QFileDialog::getOpenFileName(nullptr, tr("Import Data"), ".", "All Files(*.*)");
    if (file.isEmpty())
        return;

    fileImported(file);
    Utils::PointsReader* reader = new Utils::PointsReader(file);
    reader->setAutoDelete(false);
    connect(reader, SIGNAL(finished()), this, SLOT(onPointsReaderFinished()));
    QThreadPool::globalInstance()->start(reader);
}

vtkTable* PlotWindow::convertModelToVtkTable()
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

void PlotWindow::fileImported(const QString& file)
{
    Q_UNUSED(file)
    // subclass update ui
}


