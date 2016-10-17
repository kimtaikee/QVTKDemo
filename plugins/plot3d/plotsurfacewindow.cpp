#include "plotsurfacewindow.h"
#include "ui_tablewidgetwindow.h"

#include <utils/tablemodel.h>
#include <utils/doublespinboxdelegate.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkplot3dwidget.h>
#include <vtkutils/modeltovtktableconverter.h>

#include <vtkTable.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkPlotSurface.h>
#include <vtkChartXYZ.h>

PlotSurfaceWindow::PlotSurfaceWindow(QWidget *parent) :
    PlotWindow(parent),
    m_ui(new Ui::TableWidgetWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Plot Surface"));

    m_plotWidget = new VtkUtils::VtkPlot3DWidget(this);
    m_ui->verticalLayout->addWidget(m_plotWidget);

    m_ui->tableView->setModel(model());
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);
    connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    connect(model(), SIGNAL(rowsChanged(int,int)), this, SLOT(onRowsChanged(int,int)));

    m_ui->rowsSpinBox->setValue(sModel->rowCount());
//    delayFire();
    fireupModelToVtkTableConverter();

#if 0
    // Create a surface
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
    float numPoints = 200;
    float inc = 9.424778 / (numPoints - 1);
    for (float i = 0; i < numPoints; ++i) {
       vtkSmartPointer<vtkFloatArray> arr = vtkSmartPointer<vtkFloatArray>::New();
       table->AddColumn(arr.GetPointer());
    }

    table->SetNumberOfRows(numPoints);
    for (float i = 0; i < numPoints; ++i)
    {
        float x = i * inc;
        for (float j = 0; j < numPoints; ++j)
        {
            float y  = j * inc;
            table->SetValue(i, j, sin(sqrt(x*x + y*y)));
        }
    }

    vtkSmartPointer<vtkPlotSurface> plot = vtkSmartPointer<vtkPlotSurface>::New();

    // Set up the surface plot we wish to visualize and add it to the chart.
    plot->SetXRange(0, 10.0);
    plot->SetYRange(0, 10.0);
    plot->SetInputData(table.GetPointer());

    vtkChartXYZ* chart = vtkChartXYZ::SafeDownCast(m_plotWidget->chart());
    chart->AddPlot(plot);
    chart->Update();
    chart->RecalculateBounds();
    m_plotWidget->update();
#endif
}

PlotSurfaceWindow::~PlotSurfaceWindow()
{
    delete m_ui;
}

void PlotSurfaceWindow::on_pushButton_clicked()
{
    auto model_ = model();
    model_->blockSignals(true);
    model_->random();
    model_->blockSignals(false);
    emit model_->layoutChanged();
    fireupModelToVtkTableConverter();
}

void PlotSurfaceWindow::on_importButton_clicked()
{
    importData();
}

void PlotSurfaceWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    sModel->resize(sModel->columnCount(), arg1);
    sModel->random();
    fireupModelToVtkTableConverter();
}

void PlotSurfaceWindow::onModelToVtkTableConverterFinished()
{
    VtkUtils::ModelToVtkTableConverter* converter = qobject_cast<VtkUtils::ModelToVtkTableConverter*>(sender());
    vtkTable* table = converter->table();

    vtkChartXYZ* chart = vtkChartXYZ::SafeDownCast(m_plotWidget->chart());

    if (!m_plot3d) {
        m_plot3d = vtkPlotSurface::New();
        chart->AddPlot(m_plot3d);
    }

    m_plot3d->SetInputData(table, "X", "Y", "Z", "Color");
    chart->RecalculateBounds();

    chart->Update();
    m_plotWidget->update();

    converter->deleteLater();
    table->Delete();
}

void PlotSurfaceWindow::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVtkTableConverter();
}

void PlotSurfaceWindow::onRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}

void PlotSurfaceWindow::fileImported(const QString &file)
{
    m_ui->fileLabel->setText(file);
}



