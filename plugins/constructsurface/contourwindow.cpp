#include "contourwindow.h"
#include "ui_contourwindow.h"

#include <utils/tablemodel.h>
#include <utils/modeltovectorsconverter.h>
#include <utils/pointsreader.h>
#include <utils/pointstomodelconverter.h>

#include <vtkutils/contour.h>

#include <QThreadPool>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>

ContourWindow::ContourWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ContourWindow)
{
    setWindowTitle(tr("Contour"));
    m_ui->setupUi(this);
    m_ui->tableView->setAlternatingRowColors(true);

    m_model = new Utils::TableModel(4, 100);
    m_ui->rowsSpinbox->setValue(100);
    m_model->setHorizontalHeaderData(QVariantList() << "X" << "Y" << "Z" << "V");
    m_model->random();

    m_contour = new VtkUtils::Contour(this);
    m_ui->verticalLayout->addWidget(m_contour);

    m_ui->tableView->setModel(m_model);

    fireupModelToVectorsConverter();
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}

ContourWindow::~ContourWindow()
{
    delete m_ui;
}

void ContourWindow::onModelToVectorsConverterFinished()
{
    qDebug() << "setting new vectors";

    Utils::ModelToVectorsConverter* converter = qobject_cast<Utils::ModelToVectorsConverter*>(sender());
    QList<Utils::Vector4F> vectors = converter->vectors();

    if (vectors.isEmpty())
        return;

    m_contour->setVectors(vectors);

    converter->deleteLater();
}

void ContourWindow::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    fireupModelToVectorsConverter();
}

void ContourWindow::onPointsReaderFinished()
{
    Utils::PointsReader* reader = qobject_cast<Utils::PointsReader*>(sender());
    QList<Utils::Point3F> points = reader->points();
    if (points.isEmpty())
        return;

    QList<Utils::Vector4F> vectors;
    foreach (auto pot, points)
        vectors.append(Utils::Vector4F{pot.x, pot.y, pot.z, 0.0});

    m_contour->setVectors(vectors);
    reader->deleteLater();

    Utils::PointsToModelConverter* converter = new Utils::PointsToModelConverter(points, m_model);
    converter->setAutoDelete(false);
    connect(converter, SIGNAL(finished()), this, SLOT(onPointsToModelConverterFinished()));
    QThreadPool::globalInstance()->start(converter);
}

void ContourWindow::onPointsToModelConverterFinished()
{
    Utils::PointsToModelConverter* converter = qobject_cast<Utils::PointsToModelConverter*>(sender());
    emit m_model->layoutChanged();
    converter->deleteLater();
}

void ContourWindow::on_importDataButton_clicked()
{
    static QString previousDir = ".";

    QString file = QFileDialog::getOpenFileName(this, tr("Import Data"), previousDir, "All Files(*.*)");
    if (file.isEmpty())
        return;

    QFileInfo fi(file);
    previousDir = fi.canonicalPath();
    m_ui->fileEdit->setText(file);

    Utils::PointsReader* reader = new Utils::PointsReader(file);
    reader->setAutoDelete(false);
    connect(reader, SIGNAL(finished()), this, SLOT(onPointsReaderFinished()));
    QThreadPool::globalInstance()->start(reader);
}

void ContourWindow::on_pushButton_clicked()
{
    m_model->random();
    fireupModelToVectorsConverter();
}

void ContourWindow::on_rowsSpinbox_valueChanged(int arg1)
{
    m_model->resize(4, arg1);
    m_model->random();
    fireupModelToVectorsConverter();
}

void ContourWindow::fireupModelToVectorsConverter()
{
    Utils::ModelToVectorsConverter* converter = new Utils::ModelToVectorsConverter(m_model);
    connect(converter, SIGNAL(finished()), this, SLOT(onModelToVectorsConverterFinished()));
    converter->setAutoDelete(false);
    QThreadPool::globalInstance()->start(converter);
}

