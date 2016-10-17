#include "plotpoints3dwindow.h"
#include "ui_tablewidgetwindow.h"

#include <utils/tablemodel.h>
#include <utils/doublespinboxdelegate.h>
#include <utils/pointsreader.h>
#include <utils/signalblocker.h>

#include <vtkutils/vtkplot3dwidget.h>
#include <vtkutils/modeltovtktableconverter.h>

#include <vtkSmartPointer.h>
#include <vtkChartXYZ.h>
#include <vtkPlotPoints3D.h>
#include <vtkColorSeries.h>
#include <vtkTable.h>
#include <vtkPlot3D.h>

#include <QTableView>

PlotPoints3DWindow::PlotPoints3DWindow(QWidget *parent) :
    PlotWindow(parent),
    m_ui(new Ui::TableWidgetWindow)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Plot Points3D"));

    m_plotWidget = new VtkUtils::VtkPlot3DWidget(this);
    m_ui->verticalLayout->addWidget(m_plotWidget);

    m_ui->tableView->setModel(model());
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);
    connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    connect(model(), SIGNAL(rowsChanged(int,int)), this, SLOT(onRowsChanged(int,int)));

    m_ui->rowsSpinBox->setValue(sModel->rowCount());
//    delayFire();
    fireupModelToVtkTableConverter();
}

PlotPoints3DWindow::~PlotPoints3DWindow()
{
    delete m_ui;
}

void PlotPoints3DWindow::onModelToVtkTableConverterFinished()
{
    VtkUtils::ModelToVtkTableConverter* converter = qobject_cast<VtkUtils::ModelToVtkTableConverter*>(sender());
    vtkTable* table = converter->table();

    vtkChartXYZ* chart = vtkChartXYZ::SafeDownCast(m_plotWidget->chart());

    if (!m_plot3d) {
        m_plot3d = vtkPlotPoints3D::New();
        chart->AddPlot(m_plot3d);
    }

    m_plot3d->SetInputData(table, "X", "Y", "Z", "Color");
    chart->RecalculateBounds();

    chart->Update();
    m_plotWidget->update();

    converter->deleteLater();
    table->Delete();
}

void PlotPoints3DWindow::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVtkTableConverter();
}

void PlotPoints3DWindow::onRowsChanged(int oldRows, int newRows)
{
    Q_UNUSED(oldRows)
    Utils::SignalBlocker sb(m_ui->rowsSpinBox);
    m_ui->rowsSpinBox->setValue(newRows);
}

void PlotPoints3DWindow::fileImported(const QString &file)
{
    m_ui->fileLabel->setText(file);
}

void PlotPoints3DWindow::on_pushButton_clicked()
{
    auto model_ = model();
    model_->blockSignals(true);
    model_->random();
    model_->blockSignals(false);
    emit model_->layoutChanged();
    fireupModelToVtkTableConverter();
}

void PlotPoints3DWindow::on_importButton_clicked()
{
    importData();
}

void PlotPoints3DWindow::on_rowsSpinBox_valueChanged(int arg1)
{
    sModel->resize(sModel->columnCount(), arg1);
    sModel->random();
    fireupModelToVtkTableConverter();
}
