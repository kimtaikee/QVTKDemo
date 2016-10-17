#include "dialog.h"
#include "ui_dialog.h"

#include <utils/tablemodel.h>
#include <utils/doublespinboxdelegate.h>
#include <utils/pointsreader.h>
#include <utils/pointstomodelconverter.h>
#include <utils/modeltopointsconverter.h>

#include <vtkutils/surface.h>

#include <QFileDialog>
#include <QThreadPool>
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::Dialog)
{
    m_ui->setupUi(this);

    m_vtkWidget = new VtkUtils::Surface(this);
    m_ui->verticalLayout->addWidget(m_vtkWidget);
    m_ui->tableView->setAlternatingRowColors(true);

    m_model = new Utils::TableModel(3, 100);
    m_ui->rowsSpinbox->setValue(100);
    m_model->setHorizontalHeaderData(QVariantList() << "X" << "Y" << "Z");
    m_model->random();
    m_ui->tableView->setModel(m_model);
    m_ui->tableView->setItemDelegate(new Utils::DoubleSpinBoxDelegate);

    setWindowTitle(tr("Construct Surface"));

    fireupModelToPointsConverter();
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}

Dialog::~Dialog()
{
    delete m_ui;
}

void Dialog::on_pushButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(nullptr, tr("Import Data"), ".", "All Files(*.*)");
    if (file.isEmpty())
        return;

    m_ui->lineEdit->setText(file);
    Utils::PointsReader* reader = new Utils::PointsReader(file);
    reader->setAutoDelete(false);
    connect(reader, SIGNAL(finished()), this, SLOT(onPointsReaderFinished()));
    QThreadPool::globalInstance()->start(reader);
}

void Dialog::onPointsReaderFinished()
{
    Utils::PointsReader* reader = qobject_cast<Utils::PointsReader*>(sender());
    QList<Utils::Point3F> points = reader->points();
    m_vtkWidget->setPoints(points);
    reader->deleteLater();

    if (points.isEmpty())
        return;

    Utils::PointsToModelConverter* converter = new Utils::PointsToModelConverter(points, m_model);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onPointsToModelConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

void Dialog::onPointsToModelConverterFinished()
{
    Utils::PointsToModelConverter* converter = qobject_cast<Utils::PointsToModelConverter*>(sender());
    emit m_model->layoutChanged();
    converter->deleteLater();
}

void Dialog::onModelToPointsConverterFinished()
{
    qDebug() << "Setting new points.";
    Utils::ModelToPointsConverter* converter = qobject_cast<Utils::ModelToPointsConverter*>(sender());

    QList<Utils::Point3F> points = converter->points();
    m_vtkWidget->setPoints(points);

    converter->deleteLater();
}

void Dialog::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToPointsConverter();
}

void Dialog::on_pushButton_2_clicked()
{
    m_model->random();
    fireupModelToPointsConverter();
}

void Dialog::on_rowsSpinbox_valueChanged(int arg1)
{
    m_model->resize(3, arg1);
    m_model->random();
    fireupModelToPointsConverter();
}

void Dialog::fireupModelToPointsConverter()
{
    Utils::ModelToPointsConverter* converter = new Utils::ModelToPointsConverter(m_model);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onModelToPointsConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

